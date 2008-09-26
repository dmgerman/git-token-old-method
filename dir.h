begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|DIR_H
end_ifndef
begin_define
DECL|macro|DIR_H
define|#
directive|define
name|DIR_H
end_define
begin_struct
DECL|struct|dir_entry
struct|struct
name|dir_entry
block|{
DECL|member|len
name|unsigned
name|int
name|len
decl_stmt|;
DECL|member|name
name|char
name|name
index|[
name|FLEX_ARRAY
index|]
decl_stmt|;
comment|/* more */
block|}
struct|;
end_struct
begin_define
DECL|macro|EXC_FLAG_NODIR
define|#
directive|define
name|EXC_FLAG_NODIR
value|1
end_define
begin_define
DECL|macro|EXC_FLAG_NOWILDCARD
define|#
directive|define
name|EXC_FLAG_NOWILDCARD
value|2
end_define
begin_define
DECL|macro|EXC_FLAG_ENDSWITH
define|#
directive|define
name|EXC_FLAG_ENDSWITH
value|4
end_define
begin_define
DECL|macro|EXC_FLAG_MUSTBEDIR
define|#
directive|define
name|EXC_FLAG_MUSTBEDIR
value|8
end_define
begin_struct
DECL|struct|exclude_list
struct|struct
name|exclude_list
block|{
DECL|member|nr
name|int
name|nr
decl_stmt|;
DECL|member|alloc
name|int
name|alloc
decl_stmt|;
DECL|struct|exclude
struct|struct
name|exclude
block|{
DECL|member|pattern
specifier|const
name|char
modifier|*
name|pattern
decl_stmt|;
DECL|member|patternlen
name|int
name|patternlen
decl_stmt|;
DECL|member|base
specifier|const
name|char
modifier|*
name|base
decl_stmt|;
DECL|member|baselen
name|int
name|baselen
decl_stmt|;
DECL|member|to_exclude
name|int
name|to_exclude
decl_stmt|;
DECL|member|flags
name|int
name|flags
decl_stmt|;
block|}
DECL|member|excludes
modifier|*
modifier|*
name|excludes
struct|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|exclude_stack
struct|struct
name|exclude_stack
block|{
DECL|member|prev
name|struct
name|exclude_stack
modifier|*
name|prev
decl_stmt|;
DECL|member|filebuf
name|char
modifier|*
name|filebuf
decl_stmt|;
DECL|member|baselen
name|int
name|baselen
decl_stmt|;
DECL|member|exclude_ix
name|int
name|exclude_ix
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|dir_struct
struct|struct
name|dir_struct
block|{
DECL|member|nr
DECL|member|alloc
name|int
name|nr
decl_stmt|,
name|alloc
decl_stmt|;
DECL|member|ignored_nr
DECL|member|ignored_alloc
name|int
name|ignored_nr
decl_stmt|,
name|ignored_alloc
decl_stmt|;
DECL|member|show_ignored
name|unsigned
name|int
name|show_ignored
range|:
literal|1
decl_stmt|,
DECL|member|show_other_directories
name|show_other_directories
range|:
literal|1
decl_stmt|,
DECL|member|hide_empty_directories
name|hide_empty_directories
range|:
literal|1
decl_stmt|,
DECL|member|no_gitlinks
name|no_gitlinks
range|:
literal|1
decl_stmt|,
DECL|member|collect_ignored
name|collect_ignored
range|:
literal|1
decl_stmt|;
DECL|member|entries
name|struct
name|dir_entry
modifier|*
modifier|*
name|entries
decl_stmt|;
DECL|member|ignored
name|struct
name|dir_entry
modifier|*
modifier|*
name|ignored
decl_stmt|;
comment|/* Exclude info */
DECL|member|exclude_per_dir
specifier|const
name|char
modifier|*
name|exclude_per_dir
decl_stmt|;
DECL|member|exclude_list
name|struct
name|exclude_list
name|exclude_list
index|[
literal|3
index|]
decl_stmt|;
comment|/* 	 * We maintain three exclude pattern lists: 	 * EXC_CMDL lists patterns explicitly given on the command line. 	 * EXC_DIRS lists patterns obtained from per-directory ignore files. 	 * EXC_FILE lists patterns from fallback ignore files. 	 */
DECL|macro|EXC_CMDL
define|#
directive|define
name|EXC_CMDL
value|0
DECL|macro|EXC_DIRS
define|#
directive|define
name|EXC_DIRS
value|1
DECL|macro|EXC_FILE
define|#
directive|define
name|EXC_FILE
value|2
DECL|member|exclude_stack
name|struct
name|exclude_stack
modifier|*
name|exclude_stack
decl_stmt|;
DECL|member|basebuf
name|char
name|basebuf
index|[
name|PATH_MAX
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
specifier|extern
name|int
name|common_prefix
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|MATCHED_RECURSIVELY
define|#
directive|define
name|MATCHED_RECURSIVELY
value|1
end_define
begin_define
DECL|macro|MATCHED_FNMATCH
define|#
directive|define
name|MATCHED_FNMATCH
value|2
end_define
begin_define
DECL|macro|MATCHED_EXACTLY
define|#
directive|define
name|MATCHED_EXACTLY
value|3
end_define
begin_function_decl
specifier|extern
name|int
name|match_pathspec
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|namelen
parameter_list|,
name|int
name|prefix
parameter_list|,
name|char
modifier|*
name|seen
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|read_directory
parameter_list|(
name|struct
name|dir_struct
modifier|*
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|excluded
parameter_list|(
name|struct
name|dir_struct
modifier|*
parameter_list|,
specifier|const
name|char
modifier|*
parameter_list|,
name|int
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|add_excludes_from_file
parameter_list|(
name|struct
name|dir_struct
modifier|*
parameter_list|,
specifier|const
name|char
modifier|*
name|fname
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|add_exclude
parameter_list|(
specifier|const
name|char
modifier|*
name|string
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|,
name|struct
name|exclude_list
modifier|*
name|which
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|file_exists
parameter_list|(
specifier|const
name|char
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|struct
name|dir_entry
modifier|*
name|dir_add_name
parameter_list|(
name|struct
name|dir_struct
modifier|*
name|dir
parameter_list|,
specifier|const
name|char
modifier|*
name|pathname
parameter_list|,
name|int
name|len
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|char
modifier|*
name|get_relative_cwd
parameter_list|(
name|char
modifier|*
name|buffer
parameter_list|,
name|int
name|size
parameter_list|,
specifier|const
name|char
modifier|*
name|dir
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|is_inside_dir
parameter_list|(
specifier|const
name|char
modifier|*
name|dir
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|setup_standard_excludes
parameter_list|(
name|struct
name|dir_struct
modifier|*
name|dir
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|remove_dir_recursively
parameter_list|(
name|struct
name|strbuf
modifier|*
name|path
parameter_list|,
name|int
name|only_empty
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* tries to remove the path with empty directories along it, ignores ENOENT */
end_comment
begin_function_decl
specifier|extern
name|int
name|remove_path
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
end_unit
