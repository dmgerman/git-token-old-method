begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|CACHE_H
end_ifndef
begin_define
DECL|macro|CACHE_H
define|#
directive|define
name|CACHE_H
end_define
begin_include
include|#
directive|include
file|<unistd.h>
end_include
begin_include
include|#
directive|include
file|<stdio.h>
end_include
begin_include
include|#
directive|include
file|<sys/stat.h>
end_include
begin_include
include|#
directive|include
file|<fcntl.h>
end_include
begin_include
include|#
directive|include
file|<stddef.h>
end_include
begin_include
include|#
directive|include
file|<stdlib.h>
end_include
begin_include
include|#
directive|include
file|<stdarg.h>
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_include
include|#
directive|include
file|<errno.h>
end_include
begin_include
include|#
directive|include
file|<sys/mman.h>
end_include
begin_include
include|#
directive|include
file|<sys/param.h>
end_include
begin_include
include|#
directive|include
file|<netinet/in.h>
end_include
begin_include
include|#
directive|include
include|SHA1_HEADER
end_include
begin_include
include|#
directive|include
file|<zlib.h>
end_include
begin_if
if|#
directive|if
name|ZLIB_VERNUM
operator|<
literal|0x1200
end_if
begin_define
DECL|macro|deflateBound
define|#
directive|define
name|deflateBound
parameter_list|(
name|c
parameter_list|,
name|s
parameter_list|)
value|((s) + (((s) + 7)>> 3) + (((s) + 63)>> 6) + 11)
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_ifdef
ifdef|#
directive|ifdef
name|DT_UNKNOWN
end_ifdef
begin_define
DECL|macro|DTYPE
define|#
directive|define
name|DTYPE
parameter_list|(
name|de
parameter_list|)
value|((de)->d_type)
end_define
begin_else
else|#
directive|else
end_else
begin_define
DECL|macro|DT_UNKNOWN
define|#
directive|define
name|DT_UNKNOWN
value|0
end_define
begin_define
DECL|macro|DT_DIR
define|#
directive|define
name|DT_DIR
value|1
end_define
begin_define
DECL|macro|DT_REG
define|#
directive|define
name|DT_REG
value|2
end_define
begin_define
DECL|macro|DT_LNK
define|#
directive|define
name|DT_LNK
value|3
end_define
begin_define
DECL|macro|DTYPE
define|#
directive|define
name|DTYPE
parameter_list|(
name|de
parameter_list|)
value|DT_UNKNOWN
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/*  * Environment variables transition.  * We accept older names for now but warn.  */
end_comment
begin_function_decl
specifier|extern
name|char
modifier|*
name|gitenv_bc
parameter_list|(
specifier|const
name|char
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|gitenv
define|#
directive|define
name|gitenv
parameter_list|(
name|e
parameter_list|)
value|(getenv(e) ? : gitenv_bc(e))
end_define
begin_comment
comment|/*  * Basic data structures for the directory cache  *  * NOTE NOTE NOTE! This is all in the native CPU byte format. It's  * not even trying to be portable. It's trying to be efficient. It's  * just a cache, after all.  */
end_comment
begin_define
DECL|macro|CACHE_SIGNATURE
define|#
directive|define
name|CACHE_SIGNATURE
value|0x44495243
end_define
begin_comment
DECL|macro|CACHE_SIGNATURE
comment|/* "DIRC" */
end_comment
begin_struct
DECL|struct|cache_header
struct|struct
name|cache_header
block|{
DECL|member|hdr_signature
name|unsigned
name|int
name|hdr_signature
decl_stmt|;
DECL|member|hdr_version
name|unsigned
name|int
name|hdr_version
decl_stmt|;
DECL|member|hdr_entries
name|unsigned
name|int
name|hdr_entries
decl_stmt|;
block|}
struct|;
end_struct
begin_comment
comment|/*  * The "cache_time" is just the low 32 bits of the  * time. It doesn't matter if it overflows - we only  * check it for equality in the 32 bits we save.  */
end_comment
begin_struct
DECL|struct|cache_time
struct|struct
name|cache_time
block|{
DECL|member|sec
name|unsigned
name|int
name|sec
decl_stmt|;
DECL|member|nsec
name|unsigned
name|int
name|nsec
decl_stmt|;
block|}
struct|;
end_struct
begin_comment
comment|/*  * dev/ino/uid/gid/size are also just tracked to the low 32 bits  * Again - this is just a (very strong in practice) heuristic that  * the inode hasn't changed.  *  * We save the fields in big-endian order to allow using the  * index file over NFS transparently.  */
end_comment
begin_struct
DECL|struct|cache_entry
struct|struct
name|cache_entry
block|{
DECL|member|ce_ctime
name|struct
name|cache_time
name|ce_ctime
decl_stmt|;
DECL|member|ce_mtime
name|struct
name|cache_time
name|ce_mtime
decl_stmt|;
DECL|member|ce_dev
name|unsigned
name|int
name|ce_dev
decl_stmt|;
DECL|member|ce_ino
name|unsigned
name|int
name|ce_ino
decl_stmt|;
DECL|member|ce_mode
name|unsigned
name|int
name|ce_mode
decl_stmt|;
DECL|member|ce_uid
name|unsigned
name|int
name|ce_uid
decl_stmt|;
DECL|member|ce_gid
name|unsigned
name|int
name|ce_gid
decl_stmt|;
DECL|member|ce_size
name|unsigned
name|int
name|ce_size
decl_stmt|;
DECL|member|sha1
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
DECL|member|ce_flags
name|unsigned
name|short
name|ce_flags
decl_stmt|;
DECL|member|name
name|char
name|name
index|[
literal|0
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_define
DECL|macro|CE_NAMEMASK
define|#
directive|define
name|CE_NAMEMASK
value|(0x0fff)
end_define
begin_define
DECL|macro|CE_STAGEMASK
define|#
directive|define
name|CE_STAGEMASK
value|(0x3000)
end_define
begin_define
DECL|macro|CE_STAGESHIFT
define|#
directive|define
name|CE_STAGESHIFT
value|12
end_define
begin_define
DECL|macro|create_ce_flags
define|#
directive|define
name|create_ce_flags
parameter_list|(
name|len
parameter_list|,
name|stage
parameter_list|)
value|htons((len) | ((stage)<< CE_STAGESHIFT))
end_define
begin_define
DECL|macro|ce_namelen
define|#
directive|define
name|ce_namelen
parameter_list|(
name|ce
parameter_list|)
value|(CE_NAMEMASK& ntohs((ce)->ce_flags))
end_define
begin_define
DECL|macro|ce_size
define|#
directive|define
name|ce_size
parameter_list|(
name|ce
parameter_list|)
value|cache_entry_size(ce_namelen(ce))
end_define
begin_define
DECL|macro|ce_stage
define|#
directive|define
name|ce_stage
parameter_list|(
name|ce
parameter_list|)
value|((CE_STAGEMASK& ntohs((ce)->ce_flags))>> CE_STAGESHIFT)
end_define
begin_define
DECL|macro|ce_permissions
define|#
directive|define
name|ce_permissions
parameter_list|(
name|mode
parameter_list|)
value|(((mode)& 0100) ? 0755 : 0644)
end_define
begin_function
DECL|function|create_ce_mode
specifier|static
specifier|inline
name|unsigned
name|int
name|create_ce_mode
parameter_list|(
name|unsigned
name|int
name|mode
parameter_list|)
block|{
if|if
condition|(
name|S_ISLNK
argument_list|(
name|mode
argument_list|)
condition|)
return|return
name|htonl
argument_list|(
name|S_IFLNK
argument_list|)
return|;
return|return
name|htonl
argument_list|(
name|S_IFREG
operator||
name|ce_permissions
argument_list|(
name|mode
argument_list|)
argument_list|)
return|;
block|}
end_function
begin_define
DECL|macro|cache_entry_size
define|#
directive|define
name|cache_entry_size
parameter_list|(
name|len
parameter_list|)
value|((offsetof(struct cache_entry,name) + (len) + 8)& ~7)
end_define
begin_decl_stmt
specifier|extern
name|struct
name|cache_entry
modifier|*
modifier|*
name|active_cache
decl_stmt|;
end_decl_stmt
begin_decl_stmt
specifier|extern
name|unsigned
name|int
name|active_nr
decl_stmt|,
name|active_alloc
decl_stmt|,
name|active_cache_changed
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|GIT_DIR_ENVIRONMENT
define|#
directive|define
name|GIT_DIR_ENVIRONMENT
value|"GIT_DIR"
end_define
begin_define
DECL|macro|DEFAULT_GIT_DIR_ENVIRONMENT
define|#
directive|define
name|DEFAULT_GIT_DIR_ENVIRONMENT
value|".git"
end_define
begin_define
DECL|macro|DB_ENVIRONMENT
define|#
directive|define
name|DB_ENVIRONMENT
value|"GIT_OBJECT_DIRECTORY"
end_define
begin_define
DECL|macro|INDEX_ENVIRONMENT
define|#
directive|define
name|INDEX_ENVIRONMENT
value|"GIT_INDEX_FILE"
end_define
begin_function_decl
specifier|extern
name|char
modifier|*
name|get_object_directory
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|char
modifier|*
name|get_index_file
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|ALTERNATE_DB_ENVIRONMENT
define|#
directive|define
name|ALTERNATE_DB_ENVIRONMENT
value|"GIT_ALTERNATE_OBJECT_DIRECTORIES"
end_define
begin_define
DECL|macro|alloc_nr
define|#
directive|define
name|alloc_nr
parameter_list|(
name|x
parameter_list|)
value|(((x)+16)*3/2)
end_define
begin_comment
comment|/* Initialize and use the cache information */
end_comment
begin_function_decl
specifier|extern
name|int
name|read_cache
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|write_cache
parameter_list|(
name|int
name|newfd
parameter_list|,
name|struct
name|cache_entry
modifier|*
modifier|*
name|cache
parameter_list|,
name|int
name|entries
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cache_name_pos
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|namelen
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|ADD_CACHE_OK_TO_ADD
define|#
directive|define
name|ADD_CACHE_OK_TO_ADD
value|1
end_define
begin_comment
DECL|macro|ADD_CACHE_OK_TO_ADD
comment|/* Ok to add */
end_comment
begin_define
DECL|macro|ADD_CACHE_OK_TO_REPLACE
define|#
directive|define
name|ADD_CACHE_OK_TO_REPLACE
value|2
end_define
begin_comment
DECL|macro|ADD_CACHE_OK_TO_REPLACE
comment|/* Ok to replace file/directory */
end_comment
begin_function_decl
specifier|extern
name|int
name|add_cache_entry
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|int
name|option
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|remove_entry_at
parameter_list|(
name|int
name|pos
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|remove_file_from_cache
parameter_list|(
name|char
modifier|*
name|path
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|same_name
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|a
parameter_list|,
name|struct
name|cache_entry
modifier|*
name|b
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|ce_match_stat
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|struct
name|stat
modifier|*
name|st
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|index_fd
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|fd
parameter_list|,
name|struct
name|stat
modifier|*
name|st
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|MTIME_CHANGED
define|#
directive|define
name|MTIME_CHANGED
value|0x0001
end_define
begin_define
DECL|macro|CTIME_CHANGED
define|#
directive|define
name|CTIME_CHANGED
value|0x0002
end_define
begin_define
DECL|macro|OWNER_CHANGED
define|#
directive|define
name|OWNER_CHANGED
value|0x0004
end_define
begin_define
DECL|macro|MODE_CHANGED
define|#
directive|define
name|MODE_CHANGED
value|0x0008
end_define
begin_define
DECL|macro|INODE_CHANGED
define|#
directive|define
name|INODE_CHANGED
value|0x0010
end_define
begin_define
DECL|macro|DATA_CHANGED
define|#
directive|define
name|DATA_CHANGED
value|0x0020
end_define
begin_define
DECL|macro|TYPE_CHANGED
define|#
directive|define
name|TYPE_CHANGED
value|0x0040
end_define
begin_comment
comment|/* Return a statically allocated filename matching the sha1 signature */
end_comment
begin_function_decl
specifier|extern
name|char
modifier|*
name|sha1_file_name
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* Read and unpack a sha1 file into memory, write memory to a sha1 file */
end_comment
begin_function_decl
specifier|extern
name|void
modifier|*
name|map_sha1_file
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
modifier|*
name|unpack_sha1_file
parameter_list|(
name|void
modifier|*
name|map
parameter_list|,
name|unsigned
name|long
name|mapsize
parameter_list|,
name|char
modifier|*
name|type
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
modifier|*
name|read_sha1_file
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|char
modifier|*
name|type
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|write_sha1_file
parameter_list|(
name|char
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|len
parameter_list|,
specifier|const
name|char
modifier|*
name|type
parameter_list|,
name|unsigned
name|char
modifier|*
name|return_sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|check_sha1_signature
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|void
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
specifier|const
name|char
modifier|*
name|type
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* Read a tree into the cache */
end_comment
begin_function_decl
specifier|extern
name|int
name|read_tree
parameter_list|(
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
name|int
name|stage
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|write_sha1_from_fd
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|fd
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|has_sha1_file
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* Convert to/from hex/sha1 representation */
end_comment
begin_function_decl
specifier|extern
name|int
name|get_sha1
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|get_sha1_hex
parameter_list|(
specifier|const
name|char
modifier|*
name|hex
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|char
modifier|*
name|sha1_to_hex
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* static buffer result! */
end_comment
begin_comment
comment|/* General helper functions */
end_comment
begin_function_decl
specifier|extern
name|void
name|usage
parameter_list|(
specifier|const
name|char
modifier|*
name|err
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|die
parameter_list|(
specifier|const
name|char
modifier|*
name|err
parameter_list|,
modifier|...
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|error
parameter_list|(
specifier|const
name|char
modifier|*
name|err
parameter_list|,
modifier|...
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cache_name_compare
parameter_list|(
specifier|const
name|char
modifier|*
name|name1
parameter_list|,
name|int
name|len1
parameter_list|,
specifier|const
name|char
modifier|*
name|name2
parameter_list|,
name|int
name|len2
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
modifier|*
name|read_object_with_reference
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|required_type
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1_ret
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|const
name|char
modifier|*
name|show_date
parameter_list|(
name|unsigned
name|long
name|time
parameter_list|,
name|int
name|timezone
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|parse_date
parameter_list|(
name|char
modifier|*
name|date
parameter_list|,
name|char
modifier|*
name|buf
parameter_list|,
name|int
name|bufsize
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|datestamp
parameter_list|(
name|char
modifier|*
name|buf
parameter_list|,
name|int
name|bufsize
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|xmalloc
specifier|static
specifier|inline
name|void
modifier|*
name|xmalloc
parameter_list|(
name|size_t
name|size
parameter_list|)
block|{
name|void
modifier|*
name|ret
init|=
name|malloc
argument_list|(
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
name|die
argument_list|(
literal|"Out of memory, malloc failed"
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|xrealloc
specifier|static
specifier|inline
name|void
modifier|*
name|xrealloc
parameter_list|(
name|void
modifier|*
name|ptr
parameter_list|,
name|size_t
name|size
parameter_list|)
block|{
name|void
modifier|*
name|ret
init|=
name|realloc
argument_list|(
name|ptr
argument_list|,
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
name|die
argument_list|(
literal|"Out of memory, realloc failed"
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|xcalloc
specifier|static
specifier|inline
name|void
modifier|*
name|xcalloc
parameter_list|(
name|size_t
name|nmemb
parameter_list|,
name|size_t
name|size
parameter_list|)
block|{
name|void
modifier|*
name|ret
init|=
name|calloc
argument_list|(
name|nmemb
argument_list|,
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
name|die
argument_list|(
literal|"Out of memory, calloc failed"
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* CACHE_H */
end_comment
end_unit
