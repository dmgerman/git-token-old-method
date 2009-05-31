begin_unit
begin_comment
comment|/*  * This merges the file listing in the directory cache index  * with the actual working directory list, and shows different  * combinations of the two.  *  * Copyright (C) Linus Torvalds, 2005  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"quote.h"
end_include
begin_include
include|#
directive|include
file|"dir.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"tree.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_decl_stmt
DECL|variable|abbrev
specifier|static
name|int
name|abbrev
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_deleted
specifier|static
name|int
name|show_deleted
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_cached
specifier|static
name|int
name|show_cached
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_others
specifier|static
name|int
name|show_others
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_stage
specifier|static
name|int
name|show_stage
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_unmerged
specifier|static
name|int
name|show_unmerged
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_modified
specifier|static
name|int
name|show_modified
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_killed
specifier|static
name|int
name|show_killed
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_valid_bit
specifier|static
name|int
name|show_valid_bit
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|line_terminator
specifier|static
name|int
name|line_terminator
init|=
literal|'\n'
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|prefix_len
specifier|static
name|int
name|prefix_len
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|prefix_offset
specifier|static
name|int
name|prefix_offset
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pathspec
specifier|static
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|error_unmatch
specifier|static
name|int
name|error_unmatch
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|ps_matched
specifier|static
name|char
modifier|*
name|ps_matched
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|with_tree
specifier|static
specifier|const
name|char
modifier|*
name|with_tree
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|exc_given
specifier|static
name|int
name|exc_given
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|tag_cached
specifier|static
specifier|const
name|char
modifier|*
name|tag_cached
init|=
literal|""
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|tag_unmerged
specifier|static
specifier|const
name|char
modifier|*
name|tag_unmerged
init|=
literal|""
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|tag_removed
specifier|static
specifier|const
name|char
modifier|*
name|tag_removed
init|=
literal|""
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|tag_other
specifier|static
specifier|const
name|char
modifier|*
name|tag_other
init|=
literal|""
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|tag_killed
specifier|static
specifier|const
name|char
modifier|*
name|tag_killed
init|=
literal|""
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|tag_modified
specifier|static
specifier|const
name|char
modifier|*
name|tag_modified
init|=
literal|""
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|show_dir_entry
specifier|static
name|void
name|show_dir_entry
parameter_list|(
specifier|const
name|char
modifier|*
name|tag
parameter_list|,
name|struct
name|dir_entry
modifier|*
name|ent
parameter_list|)
block|{
name|int
name|len
init|=
name|prefix_len
decl_stmt|;
name|int
name|offset
init|=
name|prefix_offset
decl_stmt|;
if|if
condition|(
name|len
operator|>=
name|ent
operator|->
name|len
condition|)
name|die
argument_list|(
literal|"git ls-files: internal error - directory entry not superset of prefix"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|match_pathspec
argument_list|(
name|pathspec
argument_list|,
name|ent
operator|->
name|name
argument_list|,
name|ent
operator|->
name|len
argument_list|,
name|len
argument_list|,
name|ps_matched
argument_list|)
condition|)
return|return;
name|fputs
argument_list|(
name|tag
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
name|write_name_quoted
argument_list|(
name|ent
operator|->
name|name
operator|+
name|offset
argument_list|,
name|stdout
argument_list|,
name|line_terminator
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_other_files
specifier|static
name|void
name|show_other_files
parameter_list|(
name|struct
name|dir_struct
modifier|*
name|dir
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|dir
operator|->
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|dir_entry
modifier|*
name|ent
init|=
name|dir
operator|->
name|entries
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|cache_name_is_other
argument_list|(
name|ent
operator|->
name|name
argument_list|,
name|ent
operator|->
name|len
argument_list|)
condition|)
continue|continue;
name|show_dir_entry
argument_list|(
name|tag_other
argument_list|,
name|ent
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|show_killed_files
specifier|static
name|void
name|show_killed_files
parameter_list|(
name|struct
name|dir_struct
modifier|*
name|dir
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|dir
operator|->
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|dir_entry
modifier|*
name|ent
init|=
name|dir
operator|->
name|entries
index|[
name|i
index|]
decl_stmt|;
name|char
modifier|*
name|cp
decl_stmt|,
modifier|*
name|sp
decl_stmt|;
name|int
name|pos
decl_stmt|,
name|len
decl_stmt|,
name|killed
init|=
literal|0
decl_stmt|;
for|for
control|(
name|cp
operator|=
name|ent
operator|->
name|name
init|;
name|cp
operator|-
name|ent
operator|->
name|name
operator|<
name|ent
operator|->
name|len
condition|;
name|cp
operator|=
name|sp
operator|+
literal|1
control|)
block|{
name|sp
operator|=
name|strchr
argument_list|(
name|cp
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|sp
condition|)
block|{
comment|/* If ent->name is prefix of an entry in the 				 * cache, it will be killed. 				 */
name|pos
operator|=
name|cache_name_pos
argument_list|(
name|ent
operator|->
name|name
argument_list|,
name|ent
operator|->
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
literal|0
operator|<=
name|pos
condition|)
name|die
argument_list|(
literal|"bug in show-killed-files"
argument_list|)
expr_stmt|;
name|pos
operator|=
operator|-
name|pos
operator|-
literal|1
expr_stmt|;
while|while
condition|(
name|pos
operator|<
name|active_nr
operator|&&
name|ce_stage
argument_list|(
name|active_cache
index|[
name|pos
index|]
argument_list|)
condition|)
name|pos
operator|++
expr_stmt|;
comment|/* skip unmerged */
if|if
condition|(
name|active_nr
operator|<=
name|pos
condition|)
break|break;
comment|/* pos points at a name immediately after 				 * ent->name in the cache.  Does it expect 				 * ent->name to be a directory? 				 */
name|len
operator|=
name|ce_namelen
argument_list|(
name|active_cache
index|[
name|pos
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|ent
operator|->
name|len
operator|<
name|len
operator|)
operator|&&
operator|!
name|strncmp
argument_list|(
name|active_cache
index|[
name|pos
index|]
operator|->
name|name
argument_list|,
name|ent
operator|->
name|name
argument_list|,
name|ent
operator|->
name|len
argument_list|)
operator|&&
name|active_cache
index|[
name|pos
index|]
operator|->
name|name
index|[
name|ent
operator|->
name|len
index|]
operator|==
literal|'/'
condition|)
name|killed
operator|=
literal|1
expr_stmt|;
break|break;
block|}
if|if
condition|(
literal|0
operator|<=
name|cache_name_pos
argument_list|(
name|ent
operator|->
name|name
argument_list|,
name|sp
operator|-
name|ent
operator|->
name|name
argument_list|)
condition|)
block|{
comment|/* If any of the leading directories in 				 * ent->name is registered in the cache, 				 * ent->name will be killed. 				 */
name|killed
operator|=
literal|1
expr_stmt|;
break|break;
block|}
block|}
if|if
condition|(
name|killed
condition|)
name|show_dir_entry
argument_list|(
name|tag_killed
argument_list|,
name|dir
operator|->
name|entries
index|[
name|i
index|]
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|show_ce_entry
specifier|static
name|void
name|show_ce_entry
parameter_list|(
specifier|const
name|char
modifier|*
name|tag
parameter_list|,
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|)
block|{
name|int
name|len
init|=
name|prefix_len
decl_stmt|;
name|int
name|offset
init|=
name|prefix_offset
decl_stmt|;
if|if
condition|(
name|len
operator|>=
name|ce_namelen
argument_list|(
name|ce
argument_list|)
condition|)
name|die
argument_list|(
literal|"git ls-files: internal error - cache entry not superset of prefix"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|match_pathspec
argument_list|(
name|pathspec
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|ce_namelen
argument_list|(
name|ce
argument_list|)
argument_list|,
name|len
argument_list|,
name|ps_matched
argument_list|)
condition|)
return|return;
if|if
condition|(
name|tag
operator|&&
operator|*
name|tag
operator|&&
name|show_valid_bit
operator|&&
operator|(
name|ce
operator|->
name|ce_flags
operator|&
name|CE_VALID
operator|)
condition|)
block|{
specifier|static
name|char
name|alttag
index|[
literal|4
index|]
decl_stmt|;
name|memcpy
argument_list|(
name|alttag
argument_list|,
name|tag
argument_list|,
literal|3
argument_list|)
expr_stmt|;
if|if
condition|(
name|isalpha
argument_list|(
name|tag
index|[
literal|0
index|]
argument_list|)
condition|)
name|alttag
index|[
literal|0
index|]
operator|=
name|tolower
argument_list|(
name|tag
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|tag
index|[
literal|0
index|]
operator|==
literal|'?'
condition|)
name|alttag
index|[
literal|0
index|]
operator|=
literal|'!'
expr_stmt|;
else|else
block|{
name|alttag
index|[
literal|0
index|]
operator|=
literal|'v'
expr_stmt|;
name|alttag
index|[
literal|1
index|]
operator|=
name|tag
index|[
literal|0
index|]
expr_stmt|;
name|alttag
index|[
literal|2
index|]
operator|=
literal|' '
expr_stmt|;
name|alttag
index|[
literal|3
index|]
operator|=
literal|0
expr_stmt|;
block|}
name|tag
operator|=
name|alttag
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|show_stage
condition|)
block|{
name|fputs
argument_list|(
name|tag
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|printf
argument_list|(
literal|"%s%06o %s %d\t"
argument_list|,
name|tag
argument_list|,
name|ce
operator|->
name|ce_mode
argument_list|,
name|abbrev
condition|?
name|find_unique_abbrev
argument_list|(
name|ce
operator|->
name|sha1
argument_list|,
name|abbrev
argument_list|)
else|:
name|sha1_to_hex
argument_list|(
name|ce
operator|->
name|sha1
argument_list|)
argument_list|,
name|ce_stage
argument_list|(
name|ce
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|write_name_quoted
argument_list|(
name|ce
operator|->
name|name
operator|+
name|offset
argument_list|,
name|stdout
argument_list|,
name|line_terminator
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_files
specifier|static
name|void
name|show_files
parameter_list|(
name|struct
name|dir_struct
modifier|*
name|dir
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
comment|/* For cached/deleted files we don't need to even do the readdir */
if|if
condition|(
name|show_others
operator|||
name|show_killed
condition|)
block|{
specifier|const
name|char
modifier|*
name|path
init|=
literal|"."
decl_stmt|,
modifier|*
name|base
init|=
literal|""
decl_stmt|;
name|int
name|baselen
init|=
name|prefix_len
decl_stmt|;
if|if
condition|(
name|baselen
condition|)
name|path
operator|=
name|base
operator|=
name|prefix
expr_stmt|;
name|read_directory
argument_list|(
name|dir
argument_list|,
name|path
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|,
name|pathspec
argument_list|)
expr_stmt|;
if|if
condition|(
name|show_others
condition|)
name|show_other_files
argument_list|(
name|dir
argument_list|)
expr_stmt|;
if|if
condition|(
name|show_killed
condition|)
name|show_killed_files
argument_list|(
name|dir
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|show_cached
operator||
name|show_stage
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|active_nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|i
index|]
decl_stmt|;
name|int
name|dtype
init|=
name|ce_to_dtype
argument_list|(
name|ce
argument_list|)
decl_stmt|;
if|if
condition|(
name|excluded
argument_list|(
name|dir
argument_list|,
name|ce
operator|->
name|name
argument_list|,
operator|&
name|dtype
argument_list|)
operator|!=
operator|!
operator|!
operator|(
name|dir
operator|->
name|flags
operator|&
name|DIR_SHOW_IGNORED
operator|)
condition|)
continue|continue;
if|if
condition|(
name|show_unmerged
operator|&&
operator|!
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|ce
operator|->
name|ce_flags
operator|&
name|CE_UPDATE
condition|)
continue|continue;
name|show_ce_entry
argument_list|(
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|?
name|tag_unmerged
else|:
name|tag_cached
argument_list|,
name|ce
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|show_deleted
operator||
name|show_modified
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|active_nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|i
index|]
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|int
name|err
decl_stmt|;
name|int
name|dtype
init|=
name|ce_to_dtype
argument_list|(
name|ce
argument_list|)
decl_stmt|;
if|if
condition|(
name|excluded
argument_list|(
name|dir
argument_list|,
name|ce
operator|->
name|name
argument_list|,
operator|&
name|dtype
argument_list|)
operator|!=
operator|!
operator|!
operator|(
name|dir
operator|->
name|flags
operator|&
name|DIR_SHOW_IGNORED
operator|)
condition|)
continue|continue;
if|if
condition|(
name|ce
operator|->
name|ce_flags
operator|&
name|CE_UPDATE
condition|)
continue|continue;
name|err
operator|=
name|lstat
argument_list|(
name|ce
operator|->
name|name
argument_list|,
operator|&
name|st
argument_list|)
expr_stmt|;
if|if
condition|(
name|show_deleted
operator|&&
name|err
condition|)
name|show_ce_entry
argument_list|(
name|tag_removed
argument_list|,
name|ce
argument_list|)
expr_stmt|;
if|if
condition|(
name|show_modified
operator|&&
name|ce_modified
argument_list|(
name|ce
argument_list|,
operator|&
name|st
argument_list|,
literal|0
argument_list|)
condition|)
name|show_ce_entry
argument_list|(
name|tag_modified
argument_list|,
name|ce
argument_list|)
expr_stmt|;
block|}
block|}
block|}
end_function
begin_comment
comment|/*  * Prune the index to only contain stuff starting with "prefix"  */
end_comment
begin_function
DECL|function|prune_cache
specifier|static
name|void
name|prune_cache
parameter_list|(
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|int
name|pos
init|=
name|cache_name_pos
argument_list|(
name|prefix
argument_list|,
name|prefix_len
argument_list|)
decl_stmt|;
name|unsigned
name|int
name|first
decl_stmt|,
name|last
decl_stmt|;
if|if
condition|(
name|pos
operator|<
literal|0
condition|)
name|pos
operator|=
operator|-
name|pos
operator|-
literal|1
expr_stmt|;
name|memmove
argument_list|(
name|active_cache
argument_list|,
name|active_cache
operator|+
name|pos
argument_list|,
operator|(
name|active_nr
operator|-
name|pos
operator|)
operator|*
sizeof|sizeof
argument_list|(
expr|struct
name|cache_entry
operator|*
argument_list|)
argument_list|)
expr_stmt|;
name|active_nr
operator|-=
name|pos
expr_stmt|;
name|first
operator|=
literal|0
expr_stmt|;
name|last
operator|=
name|active_nr
expr_stmt|;
while|while
condition|(
name|last
operator|>
name|first
condition|)
block|{
name|int
name|next
init|=
operator|(
name|last
operator|+
name|first
operator|)
operator|>>
literal|1
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|next
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|prefix
argument_list|,
name|prefix_len
argument_list|)
condition|)
block|{
name|first
operator|=
name|next
operator|+
literal|1
expr_stmt|;
continue|continue;
block|}
name|last
operator|=
name|next
expr_stmt|;
block|}
name|active_nr
operator|=
name|last
expr_stmt|;
block|}
end_function
begin_function
DECL|function|verify_pathspec
specifier|static
specifier|const
name|char
modifier|*
name|verify_pathspec
parameter_list|(
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
specifier|const
name|char
modifier|*
modifier|*
name|p
decl_stmt|,
modifier|*
name|n
decl_stmt|,
modifier|*
name|prev
decl_stmt|;
name|unsigned
name|long
name|max
decl_stmt|;
name|prev
operator|=
name|NULL
expr_stmt|;
name|max
operator|=
name|PATH_MAX
expr_stmt|;
for|for
control|(
name|p
operator|=
name|pathspec
init|;
operator|(
name|n
operator|=
operator|*
name|p
operator|)
operator|!=
name|NULL
condition|;
name|p
operator|++
control|)
block|{
name|int
name|i
decl_stmt|,
name|len
init|=
literal|0
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|max
condition|;
name|i
operator|++
control|)
block|{
name|char
name|c
init|=
name|n
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|prev
operator|&&
name|prev
index|[
name|i
index|]
operator|!=
name|c
condition|)
break|break;
if|if
condition|(
operator|!
name|c
operator|||
name|c
operator|==
literal|'*'
operator|||
name|c
operator|==
literal|'?'
condition|)
break|break;
if|if
condition|(
name|c
operator|==
literal|'/'
condition|)
name|len
operator|=
name|i
operator|+
literal|1
expr_stmt|;
block|}
name|prev
operator|=
name|n
expr_stmt|;
if|if
condition|(
name|len
operator|<
name|max
condition|)
block|{
name|max
operator|=
name|len
expr_stmt|;
if|if
condition|(
operator|!
name|max
condition|)
break|break;
block|}
block|}
if|if
condition|(
name|prefix_offset
operator|>
name|max
operator|||
name|memcmp
argument_list|(
name|prev
argument_list|,
name|prefix
argument_list|,
name|prefix_offset
argument_list|)
condition|)
name|die
argument_list|(
literal|"git ls-files: cannot generate relative filenames containing '..'"
argument_list|)
expr_stmt|;
name|prefix_len
operator|=
name|max
expr_stmt|;
return|return
name|max
condition|?
name|xmemdupz
argument_list|(
name|prev
argument_list|,
name|max
argument_list|)
else|:
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|strip_trailing_slash_from_submodules
specifier|static
name|void
name|strip_trailing_slash_from_submodules
parameter_list|(
name|void
parameter_list|)
block|{
specifier|const
name|char
modifier|*
modifier|*
name|p
decl_stmt|;
for|for
control|(
name|p
operator|=
name|pathspec
init|;
operator|*
name|p
operator|!=
name|NULL
condition|;
name|p
operator|++
control|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
operator|*
name|p
argument_list|)
decl_stmt|,
name|pos
decl_stmt|;
if|if
condition|(
name|len
operator|<
literal|1
operator|||
operator|(
operator|*
name|p
operator|)
index|[
name|len
operator|-
literal|1
index|]
operator|!=
literal|'/'
condition|)
continue|continue;
name|pos
operator|=
name|cache_name_pos
argument_list|(
operator|*
name|p
argument_list|,
name|len
operator|-
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|pos
operator|>=
literal|0
operator|&&
name|S_ISGITLINK
argument_list|(
name|active_cache
index|[
name|pos
index|]
operator|->
name|ce_mode
argument_list|)
condition|)
operator|*
name|p
operator|=
name|xstrndup
argument_list|(
operator|*
name|p
argument_list|,
name|len
operator|-
literal|1
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/*  * Read the tree specified with --with-tree option  * (typically, HEAD) into stage #1 and then  * squash them down to stage #0.  This is used for  * --error-unmatch to list and check the path patterns  * that were given from the command line.  We are not  * going to write this index out.  */
end_comment
begin_function
DECL|function|overlay_tree_on_cache
name|void
name|overlay_tree_on_cache
parameter_list|(
specifier|const
name|char
modifier|*
name|tree_name
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|struct
name|tree
modifier|*
name|tree
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|match
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|last_stage0
init|=
name|NULL
decl_stmt|;
name|int
name|i
decl_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|tree_name
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"tree-ish %s not found."
argument_list|,
name|tree_name
argument_list|)
expr_stmt|;
name|tree
operator|=
name|parse_tree_indirect
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree
condition|)
name|die
argument_list|(
literal|"bad tree-ish %s"
argument_list|,
name|tree_name
argument_list|)
expr_stmt|;
comment|/* Hoist the unmerged entries up to stage #3 to make room */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|active_nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|)
continue|continue;
name|ce
operator|->
name|ce_flags
operator||=
name|CE_STAGEMASK
expr_stmt|;
block|}
if|if
condition|(
name|prefix
condition|)
block|{
specifier|static
specifier|const
name|char
operator|*
operator|(
name|matchbuf
index|[
literal|2
index|]
operator|)
expr_stmt|;
name|matchbuf
index|[
literal|0
index|]
operator|=
name|prefix
expr_stmt|;
name|matchbuf
index|[
literal|1
index|]
operator|=
name|NULL
expr_stmt|;
name|match
operator|=
name|matchbuf
expr_stmt|;
block|}
else|else
name|match
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
name|read_tree
argument_list|(
name|tree
argument_list|,
literal|1
argument_list|,
name|match
argument_list|)
condition|)
name|die
argument_list|(
literal|"unable to read tree entries %s"
argument_list|,
name|tree_name
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|active_nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|i
index|]
decl_stmt|;
switch|switch
condition|(
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|)
block|{
case|case
literal|0
case|:
name|last_stage0
operator|=
name|ce
expr_stmt|;
comment|/* fallthru */
default|default:
continue|continue;
case|case
literal|1
case|:
comment|/* 			 * If there is stage #0 entry for this, we do not 			 * need to show it.  We use CE_UPDATE bit to mark 			 * such an entry. 			 */
if|if
condition|(
name|last_stage0
operator|&&
operator|!
name|strcmp
argument_list|(
name|last_stage0
operator|->
name|name
argument_list|,
name|ce
operator|->
name|name
argument_list|)
condition|)
name|ce
operator|->
name|ce_flags
operator||=
name|CE_UPDATE
expr_stmt|;
block|}
block|}
block|}
end_function
begin_function
DECL|function|report_path_error
name|int
name|report_path_error
parameter_list|(
specifier|const
name|char
modifier|*
name|ps_matched
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
parameter_list|,
name|int
name|prefix_offset
parameter_list|)
block|{
comment|/* 	 * Make sure all pathspec matched; otherwise it is an error. 	 */
name|int
name|num
decl_stmt|,
name|errors
init|=
literal|0
decl_stmt|;
for|for
control|(
name|num
operator|=
literal|0
init|;
name|pathspec
index|[
name|num
index|]
condition|;
name|num
operator|++
control|)
block|{
name|int
name|other
decl_stmt|,
name|found_dup
decl_stmt|;
if|if
condition|(
name|ps_matched
index|[
name|num
index|]
condition|)
continue|continue;
comment|/* 		 * The caller might have fed identical pathspec 		 * twice.  Do not barf on such a mistake. 		 */
for|for
control|(
name|found_dup
operator|=
name|other
operator|=
literal|0
init|;
operator|!
name|found_dup
operator|&&
name|pathspec
index|[
name|other
index|]
condition|;
name|other
operator|++
control|)
block|{
if|if
condition|(
name|other
operator|==
name|num
operator|||
operator|!
name|ps_matched
index|[
name|other
index|]
condition|)
continue|continue;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|pathspec
index|[
name|other
index|]
argument_list|,
name|pathspec
index|[
name|num
index|]
argument_list|)
condition|)
comment|/* 				 * Ok, we have a match already. 				 */
name|found_dup
operator|=
literal|1
expr_stmt|;
block|}
if|if
condition|(
name|found_dup
condition|)
continue|continue;
name|error
argument_list|(
literal|"pathspec '%s' did not match any file(s) known to git."
argument_list|,
name|pathspec
index|[
name|num
index|]
operator|+
name|prefix_offset
argument_list|)
expr_stmt|;
name|errors
operator|++
expr_stmt|;
block|}
return|return
name|errors
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|ls_files_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|ls_files_usage
index|[]
init|=
block|{
literal|"git ls-files [options] [<file>]*"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|option_parse_z
specifier|static
name|int
name|option_parse_z
parameter_list|(
specifier|const
name|struct
name|option
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
block|{
name|line_terminator
operator|=
name|unset
condition|?
literal|'\n'
else|:
literal|'\0'
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|option_parse_exclude
specifier|static
name|int
name|option_parse_exclude
parameter_list|(
specifier|const
name|struct
name|option
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
block|{
name|struct
name|exclude_list
modifier|*
name|list
init|=
name|opt
operator|->
name|value
decl_stmt|;
name|exc_given
operator|=
literal|1
expr_stmt|;
name|add_exclude
argument_list|(
name|arg
argument_list|,
literal|""
argument_list|,
literal|0
argument_list|,
name|list
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|option_parse_exclude_from
specifier|static
name|int
name|option_parse_exclude_from
parameter_list|(
specifier|const
name|struct
name|option
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
block|{
name|struct
name|dir_struct
modifier|*
name|dir
init|=
name|opt
operator|->
name|value
decl_stmt|;
name|exc_given
operator|=
literal|1
expr_stmt|;
name|add_excludes_from_file
argument_list|(
name|dir
argument_list|,
name|arg
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|option_parse_exclude_standard
specifier|static
name|int
name|option_parse_exclude_standard
parameter_list|(
specifier|const
name|struct
name|option
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
block|{
name|struct
name|dir_struct
modifier|*
name|dir
init|=
name|opt
operator|->
name|value
decl_stmt|;
name|exc_given
operator|=
literal|1
expr_stmt|;
name|setup_standard_excludes
argument_list|(
name|dir
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|cmd_ls_files
name|int
name|cmd_ls_files
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|int
name|require_work_tree
init|=
literal|0
decl_stmt|,
name|show_tag
init|=
literal|0
decl_stmt|;
name|struct
name|dir_struct
name|dir
decl_stmt|;
name|struct
name|option
name|builtin_ls_files_options
index|[]
init|=
block|{
block|{
name|OPTION_CALLBACK
block|,
literal|'z'
block|,
name|NULL
block|,
name|NULL
block|,
name|NULL
block|,
literal|"paths are separated with NUL character"
block|,
name|PARSE_OPT_NOARG
block|,
name|option_parse_z
block|}
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'t'
argument_list|,
name|NULL
argument_list|,
operator|&
name|show_tag
argument_list|,
literal|"identify the file status with tags"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'v'
argument_list|,
name|NULL
argument_list|,
operator|&
name|show_valid_bit
argument_list|,
literal|"use lowercase letters for 'assume unchanged' files"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'c'
argument_list|,
literal|"cached"
argument_list|,
operator|&
name|show_cached
argument_list|,
literal|"show cached files in the output (default)"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'d'
argument_list|,
literal|"deleted"
argument_list|,
operator|&
name|show_deleted
argument_list|,
literal|"show deleted files in the output"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'m'
argument_list|,
literal|"modified"
argument_list|,
operator|&
name|show_modified
argument_list|,
literal|"show modified files in the output"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'o'
argument_list|,
literal|"others"
argument_list|,
operator|&
name|show_others
argument_list|,
literal|"show other files in the output"
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|'i'
argument_list|,
literal|"ignored"
argument_list|,
operator|&
name|dir
operator|.
name|flags
argument_list|,
literal|"show ignored files in the output"
argument_list|,
name|DIR_SHOW_IGNORED
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'s'
argument_list|,
literal|"stage"
argument_list|,
operator|&
name|show_stage
argument_list|,
literal|"show staged contents' object name in the output"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'k'
argument_list|,
literal|"killed"
argument_list|,
operator|&
name|show_killed
argument_list|,
literal|"show files on the filesystem that need to be removed"
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"directory"
argument_list|,
operator|&
name|dir
operator|.
name|flags
argument_list|,
literal|"show 'other' directories' name only"
argument_list|,
name|DIR_SHOW_OTHER_DIRECTORIES
argument_list|)
block|,
name|OPT_NEGBIT
argument_list|(
literal|0
argument_list|,
literal|"empty-directory"
argument_list|,
operator|&
name|dir
operator|.
name|flags
argument_list|,
literal|"don't show empty directories"
argument_list|,
name|DIR_HIDE_EMPTY_DIRECTORIES
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'u'
argument_list|,
literal|"unmerged"
argument_list|,
operator|&
name|show_unmerged
argument_list|,
literal|"show unmerged files in the output"
argument_list|)
block|,
block|{
name|OPTION_CALLBACK
block|,
literal|'x'
block|,
literal|"exclude"
block|,
operator|&
name|dir
operator|.
name|exclude_list
index|[
name|EXC_CMDL
index|]
block|,
literal|"pattern"
block|,
literal|"skip files matching pattern"
block|,
literal|0
block|,
name|option_parse_exclude
block|}
block|,
block|{
name|OPTION_CALLBACK
block|,
literal|'X'
block|,
literal|"exclude-from"
block|,
operator|&
name|dir
block|,
literal|"file"
block|,
literal|"exclude patterns are read from<file>"
block|,
literal|0
block|,
name|option_parse_exclude_from
block|}
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"exclude-per-directory"
argument_list|,
operator|&
name|dir
operator|.
name|exclude_per_dir
argument_list|,
literal|"file"
argument_list|,
literal|"read additional per-directory exclude patterns in<file>"
argument_list|)
block|,
block|{
name|OPTION_CALLBACK
block|,
literal|0
block|,
literal|"exclude-standard"
block|,
operator|&
name|dir
block|,
name|NULL
block|,
literal|"add the standard git exclusions"
block|,
name|PARSE_OPT_NOARG
block|,
name|option_parse_exclude_standard
block|}
block|,
block|{
name|OPTION_SET_INT
block|,
literal|0
block|,
literal|"full-name"
block|,
operator|&
name|prefix_offset
block|,
name|NULL
block|,
literal|"make the output relative to the project top directory"
block|,
name|PARSE_OPT_NOARG
operator||
name|PARSE_OPT_NONEG
block|,
name|NULL
block|}
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"error-unmatch"
argument_list|,
operator|&
name|error_unmatch
argument_list|,
literal|"if any<file> is not in the index, treat this as an error"
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"with-tree"
argument_list|,
operator|&
name|with_tree
argument_list|,
literal|"tree-ish"
argument_list|,
literal|"pretend that paths removed since<tree-ish> are still present"
argument_list|)
block|,
name|OPT__ABBREV
argument_list|(
operator|&
name|abbrev
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
name|memset
argument_list|(
operator|&
name|dir
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|dir
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|prefix
condition|)
name|prefix_offset
operator|=
name|strlen
argument_list|(
name|prefix
argument_list|)
expr_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|builtin_ls_files_options
argument_list|,
name|ls_files_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|show_tag
operator|||
name|show_valid_bit
condition|)
block|{
name|tag_cached
operator|=
literal|"H "
expr_stmt|;
name|tag_unmerged
operator|=
literal|"M "
expr_stmt|;
name|tag_removed
operator|=
literal|"R "
expr_stmt|;
name|tag_modified
operator|=
literal|"C "
expr_stmt|;
name|tag_other
operator|=
literal|"? "
expr_stmt|;
name|tag_killed
operator|=
literal|"K "
expr_stmt|;
block|}
if|if
condition|(
name|show_modified
operator|||
name|show_others
operator|||
name|show_deleted
operator|||
operator|(
name|dir
operator|.
name|flags
operator|&
name|DIR_SHOW_IGNORED
operator|)
operator|||
name|show_killed
condition|)
name|require_work_tree
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|show_unmerged
condition|)
comment|/* 		 * There's no point in showing unmerged unless 		 * you also show the stage information. 		 */
name|show_stage
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|dir
operator|.
name|exclude_per_dir
condition|)
name|exc_given
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|require_work_tree
operator|&&
operator|!
name|is_inside_work_tree
argument_list|()
condition|)
name|setup_work_tree
argument_list|()
expr_stmt|;
name|pathspec
operator|=
name|get_pathspec
argument_list|(
name|prefix
argument_list|,
name|argv
argument_list|)
expr_stmt|;
comment|/* be nice with submodule paths ending in a slash */
name|read_cache
argument_list|()
expr_stmt|;
if|if
condition|(
name|pathspec
condition|)
name|strip_trailing_slash_from_submodules
argument_list|()
expr_stmt|;
comment|/* Verify that the pathspec matches the prefix */
if|if
condition|(
name|pathspec
condition|)
name|prefix
operator|=
name|verify_pathspec
argument_list|(
name|prefix
argument_list|)
expr_stmt|;
comment|/* Treat unmatching pathspec elements as errors */
if|if
condition|(
name|pathspec
operator|&&
name|error_unmatch
condition|)
block|{
name|int
name|num
decl_stmt|;
for|for
control|(
name|num
operator|=
literal|0
init|;
name|pathspec
index|[
name|num
index|]
condition|;
name|num
operator|++
control|)
empty_stmt|;
name|ps_matched
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
name|num
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|(
name|dir
operator|.
name|flags
operator|&
name|DIR_SHOW_IGNORED
operator|)
operator|&&
operator|!
name|exc_given
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s: --ignored needs some exclude pattern\n"
argument_list|,
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
comment|/* With no flags, we default to showing the cached files */
if|if
condition|(
operator|!
operator|(
name|show_stage
operator||
name|show_deleted
operator||
name|show_others
operator||
name|show_unmerged
operator||
name|show_killed
operator||
name|show_modified
operator|)
condition|)
name|show_cached
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|prefix
condition|)
name|prune_cache
argument_list|(
name|prefix
argument_list|)
expr_stmt|;
if|if
condition|(
name|with_tree
condition|)
block|{
comment|/* 		 * Basic sanity check; show-stages and show-unmerged 		 * would not make any sense with this option. 		 */
if|if
condition|(
name|show_stage
operator|||
name|show_unmerged
condition|)
name|die
argument_list|(
literal|"ls-files --with-tree is incompatible with -s or -u"
argument_list|)
expr_stmt|;
name|overlay_tree_on_cache
argument_list|(
name|with_tree
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
block|}
name|show_files
argument_list|(
operator|&
name|dir
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
if|if
condition|(
name|ps_matched
condition|)
block|{
name|int
name|bad
decl_stmt|;
name|bad
operator|=
name|report_path_error
argument_list|(
name|ps_matched
argument_list|,
name|pathspec
argument_list|,
name|prefix_offset
argument_list|)
expr_stmt|;
if|if
condition|(
name|bad
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Did you forget to 'git add'?\n"
argument_list|)
expr_stmt|;
return|return
name|bad
condition|?
literal|1
else|:
literal|0
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
