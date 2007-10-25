begin_unit
begin_comment
comment|/*  * Copyright (C) 2005 Junio C Hamano  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"diffcore.h"
end_include
begin_include
include|#
directive|include
file|"hash.h"
end_include
begin_comment
comment|/* Table of rename/copy destinations */
end_comment
begin_struct
DECL|struct|diff_rename_dst
specifier|static
struct|struct
name|diff_rename_dst
block|{
DECL|member|two
name|struct
name|diff_filespec
modifier|*
name|two
decl_stmt|;
DECL|member|pair
name|struct
name|diff_filepair
modifier|*
name|pair
decl_stmt|;
block|}
DECL|variable|rename_dst
modifier|*
name|rename_dst
struct|;
end_struct
begin_decl_stmt
DECL|variable|rename_dst_nr
DECL|variable|rename_dst_alloc
specifier|static
name|int
name|rename_dst_nr
decl_stmt|,
name|rename_dst_alloc
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|locate_rename_dst
specifier|static
name|struct
name|diff_rename_dst
modifier|*
name|locate_rename_dst
parameter_list|(
name|struct
name|diff_filespec
modifier|*
name|two
parameter_list|,
name|int
name|insert_ok
parameter_list|)
block|{
name|int
name|first
decl_stmt|,
name|last
decl_stmt|;
name|first
operator|=
literal|0
expr_stmt|;
name|last
operator|=
name|rename_dst_nr
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
name|diff_rename_dst
modifier|*
name|dst
init|=
operator|&
operator|(
name|rename_dst
index|[
name|next
index|]
operator|)
decl_stmt|;
name|int
name|cmp
init|=
name|strcmp
argument_list|(
name|two
operator|->
name|path
argument_list|,
name|dst
operator|->
name|two
operator|->
name|path
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|cmp
condition|)
return|return
name|dst
return|;
if|if
condition|(
name|cmp
operator|<
literal|0
condition|)
block|{
name|last
operator|=
name|next
expr_stmt|;
continue|continue;
block|}
name|first
operator|=
name|next
operator|+
literal|1
expr_stmt|;
block|}
comment|/* not found */
if|if
condition|(
operator|!
name|insert_ok
condition|)
return|return
name|NULL
return|;
comment|/* insert to make it at "first" */
if|if
condition|(
name|rename_dst_alloc
operator|<=
name|rename_dst_nr
condition|)
block|{
name|rename_dst_alloc
operator|=
name|alloc_nr
argument_list|(
name|rename_dst_alloc
argument_list|)
expr_stmt|;
name|rename_dst
operator|=
name|xrealloc
argument_list|(
name|rename_dst
argument_list|,
name|rename_dst_alloc
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|rename_dst
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|rename_dst_nr
operator|++
expr_stmt|;
if|if
condition|(
name|first
operator|<
name|rename_dst_nr
condition|)
name|memmove
argument_list|(
name|rename_dst
operator|+
name|first
operator|+
literal|1
argument_list|,
name|rename_dst
operator|+
name|first
argument_list|,
operator|(
name|rename_dst_nr
operator|-
name|first
operator|-
literal|1
operator|)
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|rename_dst
argument_list|)
argument_list|)
expr_stmt|;
name|rename_dst
index|[
name|first
index|]
operator|.
name|two
operator|=
name|alloc_filespec
argument_list|(
name|two
operator|->
name|path
argument_list|)
expr_stmt|;
name|fill_filespec
argument_list|(
name|rename_dst
index|[
name|first
index|]
operator|.
name|two
argument_list|,
name|two
operator|->
name|sha1
argument_list|,
name|two
operator|->
name|mode
argument_list|)
expr_stmt|;
name|rename_dst
index|[
name|first
index|]
operator|.
name|pair
operator|=
name|NULL
expr_stmt|;
return|return
operator|&
operator|(
name|rename_dst
index|[
name|first
index|]
operator|)
return|;
block|}
end_function
begin_comment
comment|/* Table of rename/copy src files */
end_comment
begin_struct
DECL|struct|diff_rename_src
specifier|static
struct|struct
name|diff_rename_src
block|{
DECL|member|one
name|struct
name|diff_filespec
modifier|*
name|one
decl_stmt|;
DECL|member|score
name|unsigned
name|short
name|score
decl_stmt|;
comment|/* to remember the break score */
block|}
DECL|variable|rename_src
modifier|*
name|rename_src
struct|;
end_struct
begin_decl_stmt
DECL|variable|rename_src_nr
DECL|variable|rename_src_alloc
specifier|static
name|int
name|rename_src_nr
decl_stmt|,
name|rename_src_alloc
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|register_rename_src
specifier|static
name|struct
name|diff_rename_src
modifier|*
name|register_rename_src
parameter_list|(
name|struct
name|diff_filespec
modifier|*
name|one
parameter_list|,
name|unsigned
name|short
name|score
parameter_list|)
block|{
name|int
name|first
decl_stmt|,
name|last
decl_stmt|;
name|first
operator|=
literal|0
expr_stmt|;
name|last
operator|=
name|rename_src_nr
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
name|diff_rename_src
modifier|*
name|src
init|=
operator|&
operator|(
name|rename_src
index|[
name|next
index|]
operator|)
decl_stmt|;
name|int
name|cmp
init|=
name|strcmp
argument_list|(
name|one
operator|->
name|path
argument_list|,
name|src
operator|->
name|one
operator|->
name|path
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|cmp
condition|)
return|return
name|src
return|;
if|if
condition|(
name|cmp
operator|<
literal|0
condition|)
block|{
name|last
operator|=
name|next
expr_stmt|;
continue|continue;
block|}
name|first
operator|=
name|next
operator|+
literal|1
expr_stmt|;
block|}
comment|/* insert to make it at "first" */
if|if
condition|(
name|rename_src_alloc
operator|<=
name|rename_src_nr
condition|)
block|{
name|rename_src_alloc
operator|=
name|alloc_nr
argument_list|(
name|rename_src_alloc
argument_list|)
expr_stmt|;
name|rename_src
operator|=
name|xrealloc
argument_list|(
name|rename_src
argument_list|,
name|rename_src_alloc
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|rename_src
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|rename_src_nr
operator|++
expr_stmt|;
if|if
condition|(
name|first
operator|<
name|rename_src_nr
condition|)
name|memmove
argument_list|(
name|rename_src
operator|+
name|first
operator|+
literal|1
argument_list|,
name|rename_src
operator|+
name|first
argument_list|,
operator|(
name|rename_src_nr
operator|-
name|first
operator|-
literal|1
operator|)
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|rename_src
argument_list|)
argument_list|)
expr_stmt|;
name|rename_src
index|[
name|first
index|]
operator|.
name|one
operator|=
name|one
expr_stmt|;
name|rename_src
index|[
name|first
index|]
operator|.
name|score
operator|=
name|score
expr_stmt|;
return|return
operator|&
operator|(
name|rename_src
index|[
name|first
index|]
operator|)
return|;
block|}
end_function
begin_function
DECL|function|basename_same
specifier|static
name|int
name|basename_same
parameter_list|(
name|struct
name|diff_filespec
modifier|*
name|src
parameter_list|,
name|struct
name|diff_filespec
modifier|*
name|dst
parameter_list|)
block|{
name|int
name|src_len
init|=
name|strlen
argument_list|(
name|src
operator|->
name|path
argument_list|)
decl_stmt|,
name|dst_len
init|=
name|strlen
argument_list|(
name|dst
operator|->
name|path
argument_list|)
decl_stmt|;
while|while
condition|(
name|src_len
operator|&&
name|dst_len
condition|)
block|{
name|char
name|c1
init|=
name|src
operator|->
name|path
index|[
operator|--
name|src_len
index|]
decl_stmt|;
name|char
name|c2
init|=
name|dst
operator|->
name|path
index|[
operator|--
name|dst_len
index|]
decl_stmt|;
if|if
condition|(
name|c1
operator|!=
name|c2
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|c1
operator|==
literal|'/'
condition|)
return|return
literal|1
return|;
block|}
return|return
operator|(
operator|!
name|src_len
operator|||
name|src
operator|->
name|path
index|[
name|src_len
operator|-
literal|1
index|]
operator|==
literal|'/'
operator|)
operator|&&
operator|(
operator|!
name|dst_len
operator|||
name|dst
operator|->
name|path
index|[
name|dst_len
operator|-
literal|1
index|]
operator|==
literal|'/'
operator|)
return|;
block|}
end_function
begin_struct
DECL|struct|diff_score
struct|struct
name|diff_score
block|{
DECL|member|src
name|int
name|src
decl_stmt|;
comment|/* index in rename_src */
DECL|member|dst
name|int
name|dst
decl_stmt|;
comment|/* index in rename_dst */
DECL|member|score
name|int
name|score
decl_stmt|;
DECL|member|name_score
name|int
name|name_score
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|estimate_similarity
specifier|static
name|int
name|estimate_similarity
parameter_list|(
name|struct
name|diff_filespec
modifier|*
name|src
parameter_list|,
name|struct
name|diff_filespec
modifier|*
name|dst
parameter_list|,
name|int
name|minimum_score
parameter_list|)
block|{
comment|/* src points at a file that existed in the original tree (or 	 * optionally a file in the destination tree) and dst points 	 * at a newly created file.  They may be quite similar, in which 	 * case we want to say src is renamed to dst or src is copied into 	 * dst, and then some edit has been applied to dst. 	 * 	 * Compare them and return how similar they are, representing 	 * the score as an integer between 0 and MAX_SCORE. 	 * 	 * When there is an exact match, it is considered a better 	 * match than anything else; the destination does not even 	 * call into this function in that case. 	 */
name|unsigned
name|long
name|max_size
decl_stmt|,
name|delta_size
decl_stmt|,
name|base_size
decl_stmt|,
name|src_copied
decl_stmt|,
name|literal_added
decl_stmt|;
name|unsigned
name|long
name|delta_limit
decl_stmt|;
name|int
name|score
decl_stmt|;
comment|/* We deal only with regular files.  Symlink renames are handled 	 * only when they are exact matches --- in other words, no edits 	 * after renaming. 	 */
if|if
condition|(
operator|!
name|S_ISREG
argument_list|(
name|src
operator|->
name|mode
argument_list|)
operator|||
operator|!
name|S_ISREG
argument_list|(
name|dst
operator|->
name|mode
argument_list|)
condition|)
return|return
literal|0
return|;
name|max_size
operator|=
operator|(
operator|(
name|src
operator|->
name|size
operator|>
name|dst
operator|->
name|size
operator|)
condition|?
name|src
operator|->
name|size
else|:
name|dst
operator|->
name|size
operator|)
expr_stmt|;
name|base_size
operator|=
operator|(
operator|(
name|src
operator|->
name|size
operator|<
name|dst
operator|->
name|size
operator|)
condition|?
name|src
operator|->
name|size
else|:
name|dst
operator|->
name|size
operator|)
expr_stmt|;
name|delta_size
operator|=
name|max_size
operator|-
name|base_size
expr_stmt|;
comment|/* We would not consider edits that change the file size so 	 * drastically.  delta_size must be smaller than 	 * (MAX_SCORE-minimum_score)/MAX_SCORE * min(src->size, dst->size). 	 * 	 * Note that base_size == 0 case is handled here already 	 * and the final score computation below would not have a 	 * divide-by-zero issue. 	 */
if|if
condition|(
name|base_size
operator|*
operator|(
name|MAX_SCORE
operator|-
name|minimum_score
operator|)
operator|<
name|delta_size
operator|*
name|MAX_SCORE
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|(
operator|!
name|src
operator|->
name|cnt_data
operator|&&
name|diff_populate_filespec
argument_list|(
name|src
argument_list|,
literal|0
argument_list|)
operator|)
operator|||
operator|(
operator|!
name|dst
operator|->
name|cnt_data
operator|&&
name|diff_populate_filespec
argument_list|(
name|dst
argument_list|,
literal|0
argument_list|)
operator|)
condition|)
return|return
literal|0
return|;
comment|/* error but caught downstream */
name|delta_limit
operator|=
call|(
name|unsigned
name|long
call|)
argument_list|(
name|base_size
operator|*
operator|(
name|MAX_SCORE
operator|-
name|minimum_score
operator|)
operator|/
name|MAX_SCORE
argument_list|)
expr_stmt|;
if|if
condition|(
name|diffcore_count_changes
argument_list|(
name|src
argument_list|,
name|dst
argument_list|,
operator|&
name|src
operator|->
name|cnt_data
argument_list|,
operator|&
name|dst
operator|->
name|cnt_data
argument_list|,
name|delta_limit
argument_list|,
operator|&
name|src_copied
argument_list|,
operator|&
name|literal_added
argument_list|)
condition|)
return|return
literal|0
return|;
comment|/* How similar are they? 	 * what percentage of material in dst are from source? 	 */
if|if
condition|(
operator|!
name|dst
operator|->
name|size
condition|)
name|score
operator|=
literal|0
expr_stmt|;
comment|/* should not happen */
else|else
name|score
operator|=
call|(
name|int
call|)
argument_list|(
name|src_copied
operator|*
name|MAX_SCORE
operator|/
name|max_size
argument_list|)
expr_stmt|;
return|return
name|score
return|;
block|}
end_function
begin_function
DECL|function|record_rename_pair
specifier|static
name|void
name|record_rename_pair
parameter_list|(
name|int
name|dst_index
parameter_list|,
name|int
name|src_index
parameter_list|,
name|int
name|score
parameter_list|)
block|{
name|struct
name|diff_filespec
modifier|*
name|src
decl_stmt|,
modifier|*
name|dst
decl_stmt|;
name|struct
name|diff_filepair
modifier|*
name|dp
decl_stmt|;
if|if
condition|(
name|rename_dst
index|[
name|dst_index
index|]
operator|.
name|pair
condition|)
name|die
argument_list|(
literal|"internal error: dst already matched."
argument_list|)
expr_stmt|;
name|src
operator|=
name|rename_src
index|[
name|src_index
index|]
operator|.
name|one
expr_stmt|;
name|src
operator|->
name|rename_used
operator|++
expr_stmt|;
name|src
operator|->
name|count
operator|++
expr_stmt|;
name|dst
operator|=
name|rename_dst
index|[
name|dst_index
index|]
operator|.
name|two
expr_stmt|;
name|dst
operator|->
name|count
operator|++
expr_stmt|;
name|dp
operator|=
name|diff_queue
argument_list|(
name|NULL
argument_list|,
name|src
argument_list|,
name|dst
argument_list|)
expr_stmt|;
name|dp
operator|->
name|renamed_pair
operator|=
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|src
operator|->
name|path
argument_list|,
name|dst
operator|->
name|path
argument_list|)
condition|)
name|dp
operator|->
name|score
operator|=
name|rename_src
index|[
name|src_index
index|]
operator|.
name|score
expr_stmt|;
else|else
name|dp
operator|->
name|score
operator|=
name|score
expr_stmt|;
name|rename_dst
index|[
name|dst_index
index|]
operator|.
name|pair
operator|=
name|dp
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * We sort the rename similarity matrix with the score, in descending  * order (the most similar first).  */
end_comment
begin_function
DECL|function|score_compare
specifier|static
name|int
name|score_compare
parameter_list|(
specifier|const
name|void
modifier|*
name|a_
parameter_list|,
specifier|const
name|void
modifier|*
name|b_
parameter_list|)
block|{
specifier|const
name|struct
name|diff_score
modifier|*
name|a
init|=
name|a_
decl_stmt|,
modifier|*
name|b
init|=
name|b_
decl_stmt|;
if|if
condition|(
name|a
operator|->
name|score
operator|==
name|b
operator|->
name|score
condition|)
return|return
name|b
operator|->
name|name_score
operator|-
name|a
operator|->
name|name_score
return|;
return|return
name|b
operator|->
name|score
operator|-
name|a
operator|->
name|score
return|;
block|}
end_function
begin_struct
DECL|struct|file_similarity
struct|struct
name|file_similarity
block|{
DECL|member|src_dst
DECL|member|index
name|int
name|src_dst
decl_stmt|,
name|index
decl_stmt|;
DECL|member|filespec
name|struct
name|diff_filespec
modifier|*
name|filespec
decl_stmt|;
DECL|member|next
name|struct
name|file_similarity
modifier|*
name|next
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|find_identical_files
specifier|static
name|int
name|find_identical_files
parameter_list|(
name|struct
name|file_similarity
modifier|*
name|src
parameter_list|,
name|struct
name|file_similarity
modifier|*
name|dst
parameter_list|)
block|{
name|int
name|renames
init|=
literal|0
decl_stmt|;
comment|/* 	 * Walk over all the destinations ... 	 */
do|do
block|{
name|struct
name|diff_filespec
modifier|*
name|one
init|=
name|dst
operator|->
name|filespec
decl_stmt|;
name|struct
name|file_similarity
modifier|*
name|p
decl_stmt|,
modifier|*
name|best
decl_stmt|;
name|int
name|i
init|=
literal|100
decl_stmt|;
comment|/* 		 * .. to find the best source match 		 */
name|best
operator|=
name|NULL
expr_stmt|;
for|for
control|(
name|p
operator|=
name|src
init|;
name|p
condition|;
name|p
operator|=
name|p
operator|->
name|next
control|)
block|{
name|struct
name|diff_filespec
modifier|*
name|two
init|=
name|p
operator|->
name|filespec
decl_stmt|;
comment|/* False hash collission? */
if|if
condition|(
name|hashcmp
argument_list|(
name|one
operator|->
name|sha1
argument_list|,
name|two
operator|->
name|sha1
argument_list|)
condition|)
continue|continue;
comment|/* Non-regular files? If so, the modes must match! */
if|if
condition|(
operator|!
name|S_ISREG
argument_list|(
name|one
operator|->
name|mode
argument_list|)
operator|||
operator|!
name|S_ISREG
argument_list|(
name|two
operator|->
name|mode
argument_list|)
condition|)
block|{
if|if
condition|(
name|one
operator|->
name|mode
operator|!=
name|two
operator|->
name|mode
condition|)
continue|continue;
block|}
name|best
operator|=
name|p
expr_stmt|;
if|if
condition|(
name|basename_same
argument_list|(
name|one
argument_list|,
name|two
argument_list|)
condition|)
break|break;
comment|/* Too many identical alternatives? Pick one */
if|if
condition|(
operator|!
operator|--
name|i
condition|)
break|break;
block|}
if|if
condition|(
name|best
condition|)
block|{
name|record_rename_pair
argument_list|(
name|dst
operator|->
name|index
argument_list|,
name|best
operator|->
name|index
argument_list|,
name|MAX_SCORE
argument_list|)
expr_stmt|;
name|renames
operator|++
expr_stmt|;
block|}
block|}
do|while
condition|(
operator|(
name|dst
operator|=
name|dst
operator|->
name|next
operator|)
operator|!=
name|NULL
condition|)
do|;
return|return
name|renames
return|;
block|}
end_function
begin_comment
comment|/*  * Note: the rest of the rename logic depends on this  * phase also populating all the filespecs for any  * entry that isn't matched up with an exact rename.  */
end_comment
begin_function
DECL|function|free_similarity_list
specifier|static
name|void
name|free_similarity_list
parameter_list|(
name|struct
name|file_similarity
modifier|*
name|p
parameter_list|)
block|{
while|while
condition|(
name|p
condition|)
block|{
name|struct
name|file_similarity
modifier|*
name|entry
init|=
name|p
decl_stmt|;
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
comment|/* Stupid special case, see note above! */
name|diff_populate_filespec
argument_list|(
name|entry
operator|->
name|filespec
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|entry
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|find_same_files
specifier|static
name|int
name|find_same_files
parameter_list|(
name|void
modifier|*
name|ptr
parameter_list|)
block|{
name|int
name|ret
decl_stmt|;
name|struct
name|file_similarity
modifier|*
name|p
init|=
name|ptr
decl_stmt|;
name|struct
name|file_similarity
modifier|*
name|src
init|=
name|NULL
decl_stmt|,
modifier|*
name|dst
init|=
name|NULL
decl_stmt|;
comment|/* Split the hash list up into sources and destinations */
do|do
block|{
name|struct
name|file_similarity
modifier|*
name|entry
init|=
name|p
decl_stmt|;
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
if|if
condition|(
name|entry
operator|->
name|src_dst
operator|<
literal|0
condition|)
block|{
name|entry
operator|->
name|next
operator|=
name|src
expr_stmt|;
name|src
operator|=
name|entry
expr_stmt|;
block|}
else|else
block|{
name|entry
operator|->
name|next
operator|=
name|dst
expr_stmt|;
name|dst
operator|=
name|entry
expr_stmt|;
block|}
block|}
do|while
condition|(
name|p
condition|)
do|;
comment|/* 	 * If we have both sources *and* destinations, see if 	 * we can match them up 	 */
name|ret
operator|=
operator|(
name|src
operator|&&
name|dst
operator|)
condition|?
name|find_identical_files
argument_list|(
name|src
argument_list|,
name|dst
argument_list|)
else|:
literal|0
expr_stmt|;
comment|/* Free the hashes and return the number of renames found */
name|free_similarity_list
argument_list|(
name|src
argument_list|)
expr_stmt|;
name|free_similarity_list
argument_list|(
name|dst
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|hash_filespec
specifier|static
name|unsigned
name|int
name|hash_filespec
parameter_list|(
name|struct
name|diff_filespec
modifier|*
name|filespec
parameter_list|)
block|{
name|unsigned
name|int
name|hash
decl_stmt|;
if|if
condition|(
operator|!
name|filespec
operator|->
name|sha1_valid
condition|)
block|{
if|if
condition|(
name|diff_populate_filespec
argument_list|(
name|filespec
argument_list|,
literal|0
argument_list|)
condition|)
return|return
literal|0
return|;
name|hash_sha1_file
argument_list|(
name|filespec
operator|->
name|data
argument_list|,
name|filespec
operator|->
name|size
argument_list|,
literal|"blob"
argument_list|,
name|filespec
operator|->
name|sha1
argument_list|)
expr_stmt|;
block|}
name|memcpy
argument_list|(
operator|&
name|hash
argument_list|,
name|filespec
operator|->
name|sha1
argument_list|,
sizeof|sizeof
argument_list|(
name|hash
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|hash
return|;
block|}
end_function
begin_function
DECL|function|insert_file_table
specifier|static
name|void
name|insert_file_table
parameter_list|(
name|struct
name|hash_table
modifier|*
name|table
parameter_list|,
name|int
name|src_dst
parameter_list|,
name|int
name|index
parameter_list|,
name|struct
name|diff_filespec
modifier|*
name|filespec
parameter_list|)
block|{
name|void
modifier|*
modifier|*
name|pos
decl_stmt|;
name|unsigned
name|int
name|hash
decl_stmt|;
name|struct
name|file_similarity
modifier|*
name|entry
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|entry
argument_list|)
argument_list|)
decl_stmt|;
name|entry
operator|->
name|src_dst
operator|=
name|src_dst
expr_stmt|;
name|entry
operator|->
name|index
operator|=
name|index
expr_stmt|;
name|entry
operator|->
name|filespec
operator|=
name|filespec
expr_stmt|;
name|entry
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
name|hash
operator|=
name|hash_filespec
argument_list|(
name|filespec
argument_list|)
expr_stmt|;
name|pos
operator|=
name|insert_hash
argument_list|(
name|hash
argument_list|,
name|entry
argument_list|,
name|table
argument_list|)
expr_stmt|;
comment|/* We already had an entry there? */
if|if
condition|(
name|pos
condition|)
block|{
name|entry
operator|->
name|next
operator|=
operator|*
name|pos
expr_stmt|;
operator|*
name|pos
operator|=
name|entry
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/*  * Find exact renames first.  *  * The first round matches up the up-to-date entries,  * and then during the second round we try to match  * cache-dirty entries as well.  */
end_comment
begin_function
DECL|function|find_exact_renames
specifier|static
name|int
name|find_exact_renames
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|struct
name|hash_table
name|file_table
decl_stmt|;
name|init_hash
argument_list|(
operator|&
name|file_table
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
name|rename_src_nr
condition|;
name|i
operator|++
control|)
name|insert_file_table
argument_list|(
operator|&
name|file_table
argument_list|,
operator|-
literal|1
argument_list|,
name|i
argument_list|,
name|rename_src
index|[
name|i
index|]
operator|.
name|one
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
name|rename_dst_nr
condition|;
name|i
operator|++
control|)
name|insert_file_table
argument_list|(
operator|&
name|file_table
argument_list|,
literal|1
argument_list|,
name|i
argument_list|,
name|rename_dst
index|[
name|i
index|]
operator|.
name|two
argument_list|)
expr_stmt|;
comment|/* Find the renames */
name|i
operator|=
name|for_each_hash
argument_list|(
operator|&
name|file_table
argument_list|,
name|find_same_files
argument_list|)
expr_stmt|;
comment|/* .. and free the hash data structure */
name|free_hash
argument_list|(
operator|&
name|file_table
argument_list|)
expr_stmt|;
return|return
name|i
return|;
block|}
end_function
begin_function
DECL|function|diffcore_rename
name|void
name|diffcore_rename
parameter_list|(
name|struct
name|diff_options
modifier|*
name|options
parameter_list|)
block|{
name|int
name|detect_rename
init|=
name|options
operator|->
name|detect_rename
decl_stmt|;
name|int
name|minimum_score
init|=
name|options
operator|->
name|rename_score
decl_stmt|;
name|int
name|rename_limit
init|=
name|options
operator|->
name|rename_limit
decl_stmt|;
name|struct
name|diff_queue_struct
modifier|*
name|q
init|=
operator|&
name|diff_queued_diff
decl_stmt|;
name|struct
name|diff_queue_struct
name|outq
decl_stmt|;
name|struct
name|diff_score
modifier|*
name|mx
decl_stmt|;
name|int
name|i
decl_stmt|,
name|j
decl_stmt|,
name|rename_count
decl_stmt|;
name|int
name|num_create
decl_stmt|,
name|num_src
decl_stmt|,
name|dst_cnt
decl_stmt|;
if|if
condition|(
operator|!
name|minimum_score
condition|)
name|minimum_score
operator|=
name|DEFAULT_RENAME_SCORE
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|q
operator|->
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|diff_filepair
modifier|*
name|p
init|=
name|q
operator|->
name|queue
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|one
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|two
argument_list|)
condition|)
continue|continue;
comment|/* unmerged */
elseif|else
if|if
condition|(
name|options
operator|->
name|single_follow
operator|&&
name|strcmp
argument_list|(
name|options
operator|->
name|single_follow
argument_list|,
name|p
operator|->
name|two
operator|->
name|path
argument_list|)
condition|)
continue|continue;
comment|/* not interested */
else|else
name|locate_rename_dst
argument_list|(
name|p
operator|->
name|two
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|two
argument_list|)
condition|)
block|{
comment|/* 			 * If the source is a broken "delete", and 			 * they did not really want to get broken, 			 * that means the source actually stays. 			 * So we increment the "rename_used" score 			 * by one, to indicate ourselves as a user 			 */
if|if
condition|(
name|p
operator|->
name|broken_pair
operator|&&
operator|!
name|p
operator|->
name|score
condition|)
name|p
operator|->
name|one
operator|->
name|rename_used
operator|++
expr_stmt|;
name|register_rename_src
argument_list|(
name|p
operator|->
name|one
argument_list|,
name|p
operator|->
name|score
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|detect_rename
operator|==
name|DIFF_DETECT_COPY
condition|)
block|{
comment|/* 			 * Increment the "rename_used" score by 			 * one, to indicate ourselves as a user. 			 */
name|p
operator|->
name|one
operator|->
name|rename_used
operator|++
expr_stmt|;
name|register_rename_src
argument_list|(
name|p
operator|->
name|one
argument_list|,
name|p
operator|->
name|score
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|rename_dst_nr
operator|==
literal|0
operator|||
name|rename_src_nr
operator|==
literal|0
condition|)
goto|goto
name|cleanup
goto|;
comment|/* nothing to do */
comment|/* 	 * This basically does a test for the rename matrix not 	 * growing larger than a "rename_limit" square matrix, ie: 	 * 	 *    rename_dst_nr * rename_src_nr> rename_limit * rename_limit 	 * 	 * but handles the potential overflow case specially (and we 	 * assume at least 32-bit integers) 	 */
if|if
condition|(
name|rename_limit
operator|<=
literal|0
operator|||
name|rename_limit
operator|>
literal|32767
condition|)
name|rename_limit
operator|=
literal|32767
expr_stmt|;
if|if
condition|(
name|rename_dst_nr
operator|>
name|rename_limit
operator|&&
name|rename_src_nr
operator|>
name|rename_limit
condition|)
goto|goto
name|cleanup
goto|;
if|if
condition|(
name|rename_dst_nr
operator|*
name|rename_src_nr
operator|>
name|rename_limit
operator|*
name|rename_limit
condition|)
goto|goto
name|cleanup
goto|;
comment|/* 	 * We really want to cull the candidates list early 	 * with cheap tests in order to avoid doing deltas. 	 */
name|rename_count
operator|=
name|find_exact_renames
argument_list|()
expr_stmt|;
comment|/* Have we run out the created file pool?  If so we can avoid 	 * doing the delta matrix altogether. 	 */
if|if
condition|(
name|rename_count
operator|==
name|rename_dst_nr
condition|)
goto|goto
name|cleanup
goto|;
if|if
condition|(
name|minimum_score
operator|==
name|MAX_SCORE
condition|)
goto|goto
name|cleanup
goto|;
name|num_create
operator|=
operator|(
name|rename_dst_nr
operator|-
name|rename_count
operator|)
expr_stmt|;
name|num_src
operator|=
name|rename_src_nr
expr_stmt|;
name|mx
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|mx
argument_list|)
operator|*
name|num_create
operator|*
name|num_src
argument_list|)
expr_stmt|;
for|for
control|(
name|dst_cnt
operator|=
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|rename_dst_nr
condition|;
name|i
operator|++
control|)
block|{
name|int
name|base
init|=
name|dst_cnt
operator|*
name|num_src
decl_stmt|;
name|struct
name|diff_filespec
modifier|*
name|two
init|=
name|rename_dst
index|[
name|i
index|]
operator|.
name|two
decl_stmt|;
if|if
condition|(
name|rename_dst
index|[
name|i
index|]
operator|.
name|pair
condition|)
continue|continue;
comment|/* dealt with exact match already. */
for|for
control|(
name|j
operator|=
literal|0
init|;
name|j
operator|<
name|rename_src_nr
condition|;
name|j
operator|++
control|)
block|{
name|struct
name|diff_filespec
modifier|*
name|one
init|=
name|rename_src
index|[
name|j
index|]
operator|.
name|one
decl_stmt|;
name|struct
name|diff_score
modifier|*
name|m
init|=
operator|&
name|mx
index|[
name|base
operator|+
name|j
index|]
decl_stmt|;
name|m
operator|->
name|src
operator|=
name|j
expr_stmt|;
name|m
operator|->
name|dst
operator|=
name|i
expr_stmt|;
name|m
operator|->
name|score
operator|=
name|estimate_similarity
argument_list|(
name|one
argument_list|,
name|two
argument_list|,
name|minimum_score
argument_list|)
expr_stmt|;
name|m
operator|->
name|name_score
operator|=
name|basename_same
argument_list|(
name|one
argument_list|,
name|two
argument_list|)
expr_stmt|;
name|diff_free_filespec_blob
argument_list|(
name|one
argument_list|)
expr_stmt|;
block|}
comment|/* We do not need the text anymore */
name|diff_free_filespec_blob
argument_list|(
name|two
argument_list|)
expr_stmt|;
name|dst_cnt
operator|++
expr_stmt|;
block|}
comment|/* cost matrix sorted by most to least similar pair */
name|qsort
argument_list|(
name|mx
argument_list|,
name|num_create
operator|*
name|num_src
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|mx
argument_list|)
argument_list|,
name|score_compare
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
name|num_create
operator|*
name|num_src
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|diff_rename_dst
modifier|*
name|dst
init|=
operator|&
name|rename_dst
index|[
name|mx
index|[
name|i
index|]
operator|.
name|dst
index|]
decl_stmt|;
if|if
condition|(
name|dst
operator|->
name|pair
condition|)
continue|continue;
comment|/* already done, either exact or fuzzy. */
if|if
condition|(
name|mx
index|[
name|i
index|]
operator|.
name|score
operator|<
name|minimum_score
condition|)
break|break;
comment|/* there is no more usable pair. */
name|record_rename_pair
argument_list|(
name|mx
index|[
name|i
index|]
operator|.
name|dst
argument_list|,
name|mx
index|[
name|i
index|]
operator|.
name|src
argument_list|,
name|mx
index|[
name|i
index|]
operator|.
name|score
argument_list|)
expr_stmt|;
name|rename_count
operator|++
expr_stmt|;
block|}
name|free
argument_list|(
name|mx
argument_list|)
expr_stmt|;
name|cleanup
label|:
comment|/* At this point, we have found some renames and copies and they 	 * are recorded in rename_dst.  The original list is still in *q. 	 */
name|outq
operator|.
name|queue
operator|=
name|NULL
expr_stmt|;
name|outq
operator|.
name|nr
operator|=
name|outq
operator|.
name|alloc
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|q
operator|->
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|diff_filepair
modifier|*
name|p
init|=
name|q
operator|->
name|queue
index|[
name|i
index|]
decl_stmt|;
name|struct
name|diff_filepair
modifier|*
name|pair_to_free
init|=
name|NULL
decl_stmt|;
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|one
argument_list|)
operator|&&
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|two
argument_list|)
condition|)
block|{
comment|/* 			 * Creation 			 * 			 * We would output this create record if it has 			 * not been turned into a rename/copy already. 			 */
name|struct
name|diff_rename_dst
modifier|*
name|dst
init|=
name|locate_rename_dst
argument_list|(
name|p
operator|->
name|two
argument_list|,
literal|0
argument_list|)
decl_stmt|;
if|if
condition|(
name|dst
operator|&&
name|dst
operator|->
name|pair
condition|)
block|{
name|diff_q
argument_list|(
operator|&
name|outq
argument_list|,
name|dst
operator|->
name|pair
argument_list|)
expr_stmt|;
name|pair_to_free
operator|=
name|p
expr_stmt|;
block|}
else|else
comment|/* no matching rename/copy source, so 				 * record this as a creation. 				 */
name|diff_q
argument_list|(
operator|&
name|outq
argument_list|,
name|p
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|one
argument_list|)
operator|&&
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|two
argument_list|)
condition|)
block|{
comment|/* 			 * Deletion 			 * 			 * We would output this delete record if: 			 * 			 * (1) this is a broken delete and the counterpart 			 *     broken create remains in the output; or 			 * (2) this is not a broken delete, and rename_dst 			 *     does not have a rename/copy to move p->one->path 			 *     out of existence. 			 * 			 * Otherwise, the counterpart broken create 			 * has been turned into a rename-edit; or 			 * delete did not have a matching create to 			 * begin with. 			 */
if|if
condition|(
name|DIFF_PAIR_BROKEN
argument_list|(
name|p
argument_list|)
condition|)
block|{
comment|/* broken delete */
name|struct
name|diff_rename_dst
modifier|*
name|dst
init|=
name|locate_rename_dst
argument_list|(
name|p
operator|->
name|one
argument_list|,
literal|0
argument_list|)
decl_stmt|;
if|if
condition|(
name|dst
operator|&&
name|dst
operator|->
name|pair
condition|)
comment|/* counterpart is now rename/copy */
name|pair_to_free
operator|=
name|p
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|p
operator|->
name|one
operator|->
name|rename_used
condition|)
comment|/* this path remains */
name|pair_to_free
operator|=
name|p
expr_stmt|;
block|}
if|if
condition|(
name|pair_to_free
condition|)
empty_stmt|;
else|else
name|diff_q
argument_list|(
operator|&
name|outq
argument_list|,
name|p
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|diff_unmodified_pair
argument_list|(
name|p
argument_list|)
condition|)
comment|/* all the usual ones need to be kept */
name|diff_q
argument_list|(
operator|&
name|outq
argument_list|,
name|p
argument_list|)
expr_stmt|;
else|else
comment|/* no need to keep unmodified pairs */
name|pair_to_free
operator|=
name|p
expr_stmt|;
if|if
condition|(
name|pair_to_free
condition|)
name|diff_free_filepair
argument_list|(
name|pair_to_free
argument_list|)
expr_stmt|;
block|}
name|diff_debug_queue
argument_list|(
literal|"done copying original"
argument_list|,
operator|&
name|outq
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|q
operator|->
name|queue
argument_list|)
expr_stmt|;
operator|*
name|q
operator|=
name|outq
expr_stmt|;
name|diff_debug_queue
argument_list|(
literal|"done collapsing"
argument_list|,
name|q
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
name|rename_dst_nr
condition|;
name|i
operator|++
control|)
name|free_filespec
argument_list|(
name|rename_dst
index|[
name|i
index|]
operator|.
name|two
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|rename_dst
argument_list|)
expr_stmt|;
name|rename_dst
operator|=
name|NULL
expr_stmt|;
name|rename_dst_nr
operator|=
name|rename_dst_alloc
operator|=
literal|0
expr_stmt|;
name|free
argument_list|(
name|rename_src
argument_list|)
expr_stmt|;
name|rename_src
operator|=
name|NULL
expr_stmt|;
name|rename_src_nr
operator|=
name|rename_src_alloc
operator|=
literal|0
expr_stmt|;
return|return;
block|}
end_function
end_unit
