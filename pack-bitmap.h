begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|PACK_BITMAP_H
end_ifndef
begin_define
DECL|macro|PACK_BITMAP_H
define|#
directive|define
name|PACK_BITMAP_H
end_define
begin_include
include|#
directive|include
file|"ewah/ewok.h"
end_include
begin_include
include|#
directive|include
file|"khash.h"
end_include
begin_include
include|#
directive|include
file|"pack-objects.h"
end_include
begin_struct
DECL|struct|bitmap_disk_entry
struct|struct
name|bitmap_disk_entry
block|{
DECL|member|object_pos
name|uint32_t
name|object_pos
decl_stmt|;
DECL|member|xor_offset
name|uint8_t
name|xor_offset
decl_stmt|;
DECL|member|flags
name|uint8_t
name|flags
decl_stmt|;
block|}
name|__attribute__
argument_list|(
operator|(
name|packed
operator|)
argument_list|)
struct|;
end_struct
begin_struct
DECL|struct|bitmap_disk_header
struct|struct
name|bitmap_disk_header
block|{
DECL|member|magic
name|char
name|magic
index|[
literal|4
index|]
decl_stmt|;
DECL|member|version
name|uint16_t
name|version
decl_stmt|;
DECL|member|options
name|uint16_t
name|options
decl_stmt|;
DECL|member|entry_count
name|uint32_t
name|entry_count
decl_stmt|;
DECL|member|checksum
name|unsigned
name|char
name|checksum
index|[
literal|20
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|BITMAP_IDX_SIGNATURE
specifier|static
specifier|const
name|char
name|BITMAP_IDX_SIGNATURE
index|[]
init|=
block|{
literal|'B'
block|,
literal|'I'
block|,
literal|'T'
block|,
literal|'M'
block|}
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|NEEDS_BITMAP
define|#
directive|define
name|NEEDS_BITMAP
value|(1u<<22)
end_define
begin_enum
DECL|enum|pack_bitmap_opts
enum|enum
name|pack_bitmap_opts
block|{
DECL|enumerator|BITMAP_OPT_FULL_DAG
name|BITMAP_OPT_FULL_DAG
init|=
literal|1
block|,
DECL|enumerator|BITMAP_OPT_HASH_CACHE
name|BITMAP_OPT_HASH_CACHE
init|=
literal|4
block|, }
enum|;
end_enum
begin_enum
DECL|enum|pack_bitmap_flags
enum|enum
name|pack_bitmap_flags
block|{
DECL|enumerator|BITMAP_FLAG_REUSE
name|BITMAP_FLAG_REUSE
init|=
literal|0x1
block|}
enum|;
end_enum
begin_typedef
DECL|typedef|show_reachable_fn
typedef|typedef
name|int
function_decl|(
modifier|*
name|show_reachable_fn
function_decl|)
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|enum
name|object_type
name|type
parameter_list|,
name|int
name|flags
parameter_list|,
name|uint32_t
name|hash
parameter_list|,
name|struct
name|packed_git
modifier|*
name|found_pack
parameter_list|,
name|off_t
name|found_offset
parameter_list|)
function_decl|;
end_typedef
begin_function_decl
name|int
name|prepare_bitmap_git
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|count_bitmap_commit_list
parameter_list|(
name|uint32_t
modifier|*
name|commits
parameter_list|,
name|uint32_t
modifier|*
name|trees
parameter_list|,
name|uint32_t
modifier|*
name|blobs
parameter_list|,
name|uint32_t
modifier|*
name|tags
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|traverse_bitmap_commit_list
parameter_list|(
name|show_reachable_fn
name|show_reachable
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|test_bitmap_walk
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|char
modifier|*
name|pack_bitmap_filename
parameter_list|(
name|struct
name|packed_git
modifier|*
name|p
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|prepare_bitmap_walk
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|reuse_partial_packfile_from_bitmap
parameter_list|(
name|struct
name|packed_git
modifier|*
modifier|*
name|packfile
parameter_list|,
name|uint32_t
modifier|*
name|entries
parameter_list|,
name|off_t
modifier|*
name|up_to
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|rebuild_existing_bitmaps
parameter_list|(
name|struct
name|packing_data
modifier|*
name|mapping
parameter_list|,
name|khash_sha1
modifier|*
name|reused_bitmaps
parameter_list|,
name|int
name|show_progress
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|bitmap_writer_show_progress
parameter_list|(
name|int
name|show
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|bitmap_writer_set_checksum
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|bitmap_writer_build_type_index
parameter_list|(
name|struct
name|pack_idx_entry
modifier|*
modifier|*
name|index
parameter_list|,
name|uint32_t
name|index_nr
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|bitmap_writer_reuse_bitmaps
parameter_list|(
name|struct
name|packing_data
modifier|*
name|to_pack
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|bitmap_writer_select_commits
parameter_list|(
name|struct
name|commit
modifier|*
modifier|*
name|indexed_commits
parameter_list|,
name|unsigned
name|int
name|indexed_commits_nr
parameter_list|,
name|int
name|max_bitmaps
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|bitmap_writer_build
parameter_list|(
name|struct
name|packing_data
modifier|*
name|to_pack
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|bitmap_writer_finish
parameter_list|(
name|struct
name|pack_idx_entry
modifier|*
modifier|*
name|index
parameter_list|,
name|uint32_t
name|index_nr
parameter_list|,
specifier|const
name|char
modifier|*
name|filename
parameter_list|,
name|uint16_t
name|options
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
end_unit
