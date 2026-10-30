/*************************************************************************
  vStrip by [maven] (maven@maven.de)
    udf.c:	routines for udf-parsing (because windows just doesn't cut it),
                    refs: udf102.pdf, udf200.pdf, ecma 167
                    (tabsize 2)
*************************************************************************/

#include "stdafx.h"
#include "udf.h"
#include <cstring>

static bool aspi_GetSectorInfo(const HANDLE hDrive, DWORD* sec_size, DWORD* max_sec)
{
    LARGE_INTEGER size;
    size.LowPart = GetFileSize(hDrive, reinterpret_cast<DWORD*>(&size.HighPart));

    *sec_size = 2048;
    *max_sec = size.QuadPart / *sec_size;

    return true;
}

static bool aspi_ReadSectors(const HANDLE hDrive, int lba, int nSectors, DWORD sec_size, BYTE* sector)
{
    DWORD nbr = 0;
    return lba * sec_size == SetFilePointer(hDrive, lba * sec_size, nullptr, FILE_BEGIN) && ReadFile(hDrive, sector, nSectors * sec_size, &nbr, nullptr);
}

[[nodiscard]] static bool udf_RangeContains(const BYTE* base, const DWORD bytes, const BYTE* ptr, const DWORD need)
{
    if (base == nullptr || ptr < base)
    {
        return false;
    }
    const DWORD offset{ static_cast<DWORD>(ptr - base) };
    return offset <= bytes && need <= bytes - offset;
}

[[nodiscard]] static bool udf_GetLBA(const tp_udf_FileEntry fe, const DWORD sec_size, const DWORD sectorBytes, DWORD* start, DWORD* end)
{
    if (fe == nullptr || sec_size == 0U || sectorBytes < sizeof(t_udf_FileEntry) || fe->LengthofAllocationDescriptors == 0U)
    {
        return false;
    }

    DWORD adSize{ 0U };
    switch (fe->ICBTag.Flags & udf_icbf_Mask)
    {
    case udf_icbf_ShortAd:
        adSize = sizeof(t_udf_short_ad);
        break;
    case udf_icbf_LongAd:
        adSize = sizeof(t_udf_long_ad);
        break;
    case udf_icbf_ExtAd:
        adSize = sizeof(t_udf_ext_ad);
        break;
    default:
        return false;
    }
    if (fe->LengthofAllocationDescriptors < adSize)
    {
        return false;
    }

    const DWORD header{ static_cast<DWORD>(offsetof(t_udf_FileEntry, ExtendedAttributes)) };
    if (fe->LengthofExtendedAttributes > sectorBytes - header || adSize > sectorBytes - header - fe->LengthofExtendedAttributes)
    {
        return false;
    }

    const BYTE* ad{ fe->ExtendedAttributes + fe->LengthofExtendedAttributes };
    DWORD length{ 0U };
    DWORD location{ 0U };
    switch (fe->ICBTag.Flags & udf_icbf_Mask)
    {
    case udf_icbf_ShortAd: {
        const tp_udf_short_ad desc{ reinterpret_cast<tp_udf_short_ad>(ad) };
        length = desc->Length;
        location = desc->Location;
        break;
    }
    case udf_icbf_LongAd: {
        const tp_udf_long_ad desc{ reinterpret_cast<tp_udf_long_ad>(ad) };
        length = desc->Length;
        location = desc->Location.Location; // ignore partition number
        break;
    }
    case udf_icbf_ExtAd: {
        const tp_udf_ext_ad desc{ reinterpret_cast<tp_udf_ext_ad>(ad) };
        length = desc->Length;
        location = desc->Location.Location; // ignore partition number
        break;
    }
    default:
        return false;
    }

    length &= udf_LengthMask;
    if (length == 0U)
    {
        return false;
    }
    const DWORD span{ (length - 1U) / sec_size };
    if (location > MAXDWORD - span)
    {
        return false;
    }
    *start = location;
    *end = location + span;
    return true;
}

tp_udf_file udf_get_root(const HANDLE hDrive, const WORD partition_number)
{
    BYTE sector[fio_SECTOR_SIZE];
    tp_udf_tag tag = reinterpret_cast<tp_udf_tag>(sector);
    DWORD sec_size, max_sec, i, j;
    DWORD MVDS_lba, MVDS_lba_end, MVDS_back_lba, MVDS_back_lba_end;
    DWORD FileDescriptorSequence_lba, FileDescriptorSequence_lba_end;
    DWORD partition_lba, parent_icb;
    tp_udf_AnchorVolumeDescriptorPointer avd;
    bool res, part_valid, vol_valid;

    if (!aspi_GetSectorInfo(hDrive, &sec_size, &max_sec))
    {
        return nullptr;
    }

    if (sec_size != fio_SECTOR_SIZE || max_sec < 256)
    {
        return nullptr;
    }

    // read AnchorVolumeDescriptorPointer at 256 (or MaxSec) (Tag == 2)
    res = aspi_ReadSectors(hDrive, 256, 1, sec_size, sector);
    if (!res || tag->TagIdentifier != udf_TAG_AnchorVolumeDescriptor)
    {
        res = aspi_ReadSectors(hDrive, max_sec, 1, sec_size, sector);
        if (!res || tag->TagIdentifier != udf_TAG_AnchorVolumeDescriptor)
        {
            return nullptr;
        }
    }

    // check Static Structures

    // get MainVolumeDescriptorSequence Location & Length
    avd = reinterpret_cast<tp_udf_AnchorVolumeDescriptorPointer>(sector);
    MVDS_lba = avd->MainVolumeDescriptorSequenceExtent.Location;
    MVDS_lba_end = MVDS_lba + (avd->MainVolumeDescriptorSequenceExtent.Length - 1) / sec_size;
    MVDS_back_lba = avd->ReserveVolumeDescriptorSequenceExtent.Location;
    MVDS_back_lba_end = MVDS_back_lba + (avd->ReserveVolumeDescriptorSequenceExtent.Length - 1) / sec_size;

    // read MVDS_Location..MVDS_Location + (MVDS_Length - 1) / SectorSize sectors

    part_valid = vol_valid = false;
    i = 1;
    do
    { // try twice (if we need to) for ReserveAnchor
        j = MVDS_lba;
        do
        {
            res = aspi_ReadSectors(hDrive, j++, 1, sec_size, sector);
            if (res)
            {
                if (tag->TagIdentifier == udf_TAG_PartitionDescriptor && !part_valid)
                { // get stuff out of partition
                    tp_udf_PartitionDescriptor par = reinterpret_cast<tp_udf_PartitionDescriptor>(sector);

                    part_valid = par->PartitionNumber == partition_number;
                    if (part_valid)
                    { // extract par->PartitionStartingLocation, par->PartitionLength
                        partition_lba = par->PartitionStartingLocation;
                    }
                }
                else if (tag->TagIdentifier == udf_TAG_LogicalVolumeDescriptor && !vol_valid)
                { // get stuff out of volume
                    tp_udf_LogicalVolumeDescriptor vol = reinterpret_cast<tp_udf_LogicalVolumeDescriptor>(sector);

                    // check_volume sector size
                    vol_valid = (vol->LogicalBlockSize == sec_size) && (partition_number == vol->FileSetDescriptorSequence.Location.PartitionNumber);
                    if (vol_valid)
                    { // extract vol->FileSetDescriptorSequence
                        FileDescriptorSequence_lba = vol->FileSetDescriptorSequence.Location.Location;
                        FileDescriptorSequence_lba_end = FileDescriptorSequence_lba + ((vol->FileSetDescriptorSequence.Length & udf_LengthMask) - 1) / sec_size;
                    }
                }
            }
            else
            {
                tag->TagIdentifier = 0;
            }
        } while (j <= MVDS_lba_end && tag->TagIdentifier != udf_TAG_TerminatingDescriptor && ((!part_valid) || (!vol_valid)));

        if ((!part_valid) || (!vol_valid))
        { // try backup
            MVDS_lba = MVDS_back_lba;
            MVDS_lba_end = MVDS_back_lba_end;
        }
    } while (i-- && ((!part_valid) || (!vol_valid)));

    if (part_valid && vol_valid)
    { // read FileSetDescriptor, get RootDir Location & Length, RootDir Length != 0
        res = aspi_ReadSectors(hDrive, FileDescriptorSequence_lba + partition_lba, 1, sec_size, sector);
        if (res && tag->TagIdentifier == udf_TAG_FileSetDescriptor)
        {
            tp_udf_FileSetDescriptor fsd = reinterpret_cast<tp_udf_FileSetDescriptor>(sector);

            if (partition_number == fsd->RootDirectoryICB.Location.PartitionNumber)
            {
                parent_icb = fsd->RootDirectoryICB.Location.Location;
                res = aspi_ReadSectors(hDrive, partition_lba + parent_icb, 1, sec_size, sector);
                if (res && tag->TagIdentifier == udf_TAG_FileEntry)
                {
                    tp_udf_FileEntry fe = reinterpret_cast<tp_udf_FileEntry>(sector);

                    if (fe->ICBTag.FileType == udf_FT_Directory)
                    {
                        tp_udf_file root = static_cast<tp_udf_file>(std::malloc(sizeof *root);

						root->partition_lba = partition_lba;
						if (!udf_GetLBA(fe, sec_size, fio_SECTOR_SIZE, &root->dir_lba, &root->dir_end_lba))
						{
                            std::free(root);
                            return nullptr;
						}
						root->dir_left = static_cast<DWORD>(fe->InformationLength); // don't want directories of more than 4gb
						root->sector = nullptr;
						root->fid = nullptr;
						root->sec_size = sec_size;
						std::strcpy(root->name, "/");
						root->is_dir = true;
						root->is_parent = false;
						return root;
                    }
                }
            }
        }
    }

    return nullptr;
}

static void udf_GetName(const BYTE* data, const DWORD len, char* target)
{
    DWORD p{ 1U };
    DWORD i{ 0U };
    const DWORD cap{ udf_MAX_NAMELEN - 1U };

    if (data == nullptr || len == 0U || !(data[0U] & 0x18))
    {
        target[0U] = '\0';
        return;
    }

    if (data[0U] & 0x10)
    { // ignore MSB of unicode16
        ++p;

        while (p < len && i < cap)
        {
            p += 2U;
            if (p >= len)
            {
                break;
            }
            target[i++] = data[p];
        }
    }
    else
    {
        while (p < len && i < cap)
        {
            target[i++] = data[p++];
        }
    }

    target[i] = '\0';
}

tp_udf_file udf_get_sub(const HANDLE hDrive, tp_udf_file f)
{
    if (f->is_dir && !f->is_parent && f->fid)
    {
        BYTE sector[fio_SECTOR_SIZE];
        tp_udf_tag tag = reinterpret_cast<tp_udf_tag>(sector);
        bool res;

        res = aspi_ReadSectors(hDrive, f->partition_lba + f->fid->ICB.Location.Location, 1, f->sec_size, sector);
        if (res && tag->TagIdentifier == udf_TAG_FileEntry)
        {
            tp_udf_FileEntry fe = reinterpret_cast<tp_udf_FileEntry>(sector);

            if (fe->ICBTag.FileType == udf_FT_Directory)
            {
                tp_udf_file newf = static_cast<tp_udf_file>(std::malloc(sizeof *newf);

				std::strcpy(newf->name, f->name); // maybe just ""?
				newf->sec_size = f->sec_size;
				newf->partition_lba = f->partition_lba;
				if (!udf_GetLBA(fe, f->sec_size, fio_SECTOR_SIZE, &newf->dir_lba, &newf->dir_end_lba))
				{
                    std::free(newf);
                    return nullptr;
				}
				newf->dir_left = static_cast<DWORD>(fe->InformationLength); // don't want directories of more than 4gb
				newf->sector = nullptr;
				newf->fid = nullptr;
				newf->is_dir = true;
				newf->is_parent = false;
				return newf;
            }
        }
    }
    return nullptr;
}

tp_udf_file udf_get_next(const HANDLE hDrive, tp_udf_file f)
{
    bool res = true;

    if (f->dir_left <= 0)
    {
        f->fid = nullptr;
        return nullptr;
    }

    if (f->fid)
    { // advance to next FileIdentifierDescriptor
        const DWORD dirBytes{ f->sec_size == 0U || f->dir_end_lba < f->dir_lba || (f->dir_end_lba - f->dir_lba + 1U) > (MAXDWORD / f->sec_size)
                                  ? 0U
                                  : f->sec_size * (f->dir_end_lba - f->dir_lba + 1U) };
        const BYTE* fidBytes{ reinterpret_cast<const BYTE*>(f->fid) };
        const DWORD fixed{ static_cast<DWORD>(sizeof(t_udf_FileIdentifierDescriptor)) };
        if (!udf_RangeContains(f->sector, dirBytes, fidBytes, fixed))
        {
            f->fid = nullptr;
            return nullptr;
        }
        const DWORD body{ static_cast<DWORD>(f->fid->LengthofImplementationUse) + f->fid->LengthofFileIdentifier };
        if (body > MAXDWORD - fixed - 3U)
        {
            f->fid = nullptr;
            return nullptr;
        }
        const DWORD ofs{ 4U * ((fixed + body + 3U) / 4U) };
        if (!udf_RangeContains(f->sector, dirBytes, fidBytes, ofs))
        {
            f->fid = nullptr;
            return nullptr;
        }

        f->fid = reinterpret_cast<tp_udf_FileIdentifierDescriptor>(fidBytes + ofs);
    }

    if (f->fid == nullptr)
    {
        if (f->sec_size == 0U || f->dir_end_lba < f->dir_lba || (f->dir_end_lba - f->dir_lba + 1U) > (MAXDWORD / f->sec_size))
        {
            f->fid = nullptr;
            return nullptr;
        }
        DWORD size = f->sec_size * (f->dir_end_lba - f->dir_lba + 1U);

        if (!f->sector)
        {
            f->sector = static_cast<BYTE*>(std::malloc(size));
            if (!f->sector)
            {
                f->fid = nullptr;
                return nullptr;
            }
        }
        res = aspi_ReadSectors(hDrive, f->partition_lba + f->dir_lba, static_cast<WORD>(f->dir_end_lba - f->dir_lba + 1), f->sec_size, f->sector);
        if (res)
        {
            f->fid = reinterpret_cast<tp_udf_FileIdentifierDescriptor>(f->sector);
        }
        else
        {
            f->fid = nullptr;
        }
    }

    if (f->fid && f->fid->DescriptorTag.TagIdentifier == udf_TAG_FileIdentifierDescriptor)
    {
        const DWORD dirBytes{ f->sec_size * (f->dir_end_lba - f->dir_lba + 1U) };
        const BYTE* fidBytes{ reinterpret_cast<const BYTE*>(f->fid) };
        const DWORD fixed{ static_cast<DWORD>(sizeof(t_udf_FileIdentifierDescriptor)) };
        const DWORD body{ static_cast<DWORD>(f->fid->LengthofImplementationUse) + f->fid->LengthofFileIdentifier };
        const DWORD ofs{ 4U * ((fixed + body + 3U) / 4U) };
        const BYTE* identifier{ f->fid->ImplementationUse + f->fid->LengthofImplementationUse };
        if (!udf_RangeContains(f->sector, dirBytes, fidBytes, fixed) || !udf_RangeContains(f->sector, dirBytes, identifier, f->fid->LengthofFileIdentifier))
        {
            f->fid = nullptr;
            return nullptr;
        }

        if (static_cast<DWORD>(f->dir_left) < ofs)
        {
            f->dir_left = 0;
        }
        else
        {
            f->dir_left -= static_cast<int>(ofs);
        }
        f->is_dir = (f->fid->FileCharacteristics & udf_FID_Directory) != 0;
        f->is_parent = (f->fid->FileCharacteristics & udf_FID_Parent) != 0;
        udf_GetName(identifier, f->fid->LengthofFileIdentifier, f->name);
        return f;
    }
    return nullptr;
}

void udf_free(tp_udf_file f)
{
    if (f)
    {
        if (f->sector)
        {
            std::free(f->sector);
        }
        std::free(f);
    }
}

#define udf_PATH_DELIMITERS "/\\"

static tp_udf_file udf_ff_traverse(const HANDLE hDrive, tp_udf_file f, char* token)
{
    while (udf_get_next(hDrive, f))
    {
        if (stricmp(token, f->name) == 0)
        {
            char* next_tok = std::strtok(nullptr, udf_PATH_DELIMITERS);

            if (!next_tok)
            {
                return f;
            } // found
            else if (f->is_dir)
            {
                tp_udf_file f2 = udf_get_sub(hDrive, f);

                if (f2)
                {
                    tp_udf_file f3 = udf_ff_traverse(hDrive, f2, next_tok);

                    if (!f3)
                    {
                        udf_free(f2);
                    }
                    return f3;
                }
            }
        }
    }
    return nullptr;
}

tp_udf_file udf_find_file(const HANDLE hDrive, const WORD partition, const char* name)
{
    tp_udf_file f = udf_get_root(hDrive, partition), f2 = nullptr;

    if (f)
    {
        char tokenline[udf_MAX_PATHLEN];
        char* token;

        if (name == nullptr || std::strlen(name) >= udf_MAX_PATHLEN)
        {
            udf_free(f);
            return nullptr;
        }
        std::memcpy(tokenline, name, std::strlen(name) + 1U);
        token = std::strtok(tokenline, udf_PATH_DELIMITERS);
        if (token)
        {
            f2 = udf_ff_traverse(hDrive, f, token);
        }
        udf_free(f);
    }
    return f2;
}

bool udf_get_lba(const HANDLE hDrive, const tp_udf_file f, DWORD* start_lba, DWORD* end_lba)
{
    if (f->fid)
    {
        BYTE sector[2048U];
        tp_udf_FileEntry fe = reinterpret_cast<tp_udf_FileEntry>(sector);
        bool res;

        res = aspi_ReadSectors(hDrive, f->partition_lba + f->fid->ICB.Location.Location, 1, f->sec_size, sector);
        if (res && fe->DescriptorTag.TagIdentifier == udf_TAG_FileEntry)
        {
            return udf_GetLBA(fe, f->sec_size, sizeof(sector), start_lba, end_lba);
        }
    }
    return false;
}
