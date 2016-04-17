begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"tree.h"
end_include
begin_include
include|#
directive|include
file|"tree-walk.h"
end_include
begin_function
DECL|function|score_missing
specifier|static
name|int
name|score_missing
parameter_list|(
name|unsigned
name|mode
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|score
decl_stmt|;
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|)
name|score
operator|=
operator|-
literal|1000
expr_stmt|;
elseif|else
if|if
condition|(
name|S_ISLNK
argument_list|(
name|mode
argument_list|)
condition|)
name|score
operator|=
operator|-
literal|500
expr_stmt|;
else|else
name|score
operator|=
operator|-
literal|50
expr_stmt|;
return|return
name|score
return|;
block|}
end_function
begin_function
DECL|function|score_differs
specifier|static
name|int
name|score_differs
parameter_list|(
name|unsigned
name|mode1
parameter_list|,
name|unsigned
name|mode2
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|score
decl_stmt|;
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode1
argument_list|)
operator|!=
name|S_ISDIR
argument_list|(
name|mode2
argument_list|)
condition|)
name|score
operator|=
operator|-
literal|100
expr_stmt|;
elseif|else
if|if
condition|(
name|S_ISLNK
argument_list|(
name|mode1
argument_list|)
operator|!=
name|S_ISLNK
argument_list|(
name|mode2
argument_list|)
condition|)
name|score
operator|=
operator|-
literal|50
expr_stmt|;
else|else
name|score
operator|=
operator|-
literal|5
expr_stmt|;
return|return
name|score
return|;
block|}
end_function
begin_function
DECL|function|score_matches
specifier|static
name|int
name|score_matches
parameter_list|(
name|unsigned
name|mode1
parameter_list|,
name|unsigned
name|mode2
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|score
decl_stmt|;
comment|/* Heh, we found SHA-1 collisions between different kind of objects */
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode1
argument_list|)
operator|!=
name|S_ISDIR
argument_list|(
name|mode2
argument_list|)
condition|)
name|score
operator|=
operator|-
literal|100
expr_stmt|;
elseif|else
if|if
condition|(
name|S_ISLNK
argument_list|(
name|mode1
argument_list|)
operator|!=
name|S_ISLNK
argument_list|(
name|mode2
argument_list|)
condition|)
name|score
operator|=
operator|-
literal|50
expr_stmt|;
elseif|else
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode1
argument_list|)
condition|)
name|score
operator|=
literal|1000
expr_stmt|;
elseif|else
if|if
condition|(
name|S_ISLNK
argument_list|(
name|mode1
argument_list|)
condition|)
name|score
operator|=
literal|500
expr_stmt|;
else|else
name|score
operator|=
literal|250
expr_stmt|;
return|return
name|score
return|;
block|}
end_function
begin_function
DECL|function|fill_tree_desc_strict
specifier|static
name|void
modifier|*
name|fill_tree_desc_strict
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|desc
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|hash
parameter_list|)
block|{
name|void
modifier|*
name|buffer
decl_stmt|;
name|enum
name|object_type
name|type
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|buffer
operator|=
name|read_sha1_file
argument_list|(
name|hash
argument_list|,
operator|&
name|type
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buffer
condition|)
name|die
argument_list|(
literal|"unable to read tree (%s)"
argument_list|,
name|sha1_to_hex
argument_list|(
name|hash
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|type
operator|!=
name|OBJ_TREE
condition|)
name|die
argument_list|(
literal|"%s is not a tree"
argument_list|,
name|sha1_to_hex
argument_list|(
name|hash
argument_list|)
argument_list|)
expr_stmt|;
name|init_tree_desc
argument_list|(
name|desc
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
return|return
name|buffer
return|;
block|}
end_function
begin_function
DECL|function|base_name_entries_compare
specifier|static
name|int
name|base_name_entries_compare
parameter_list|(
specifier|const
name|struct
name|name_entry
modifier|*
name|a
parameter_list|,
specifier|const
name|struct
name|name_entry
modifier|*
name|b
parameter_list|)
block|{
return|return
name|base_name_compare
argument_list|(
name|a
operator|->
name|path
argument_list|,
name|tree_entry_len
argument_list|(
name|a
argument_list|)
argument_list|,
name|a
operator|->
name|mode
argument_list|,
name|b
operator|->
name|path
argument_list|,
name|tree_entry_len
argument_list|(
name|b
argument_list|)
argument_list|,
name|b
operator|->
name|mode
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/*  * Inspect two trees, and give a score that tells how similar they are.  */
end_comment
begin_function
DECL|function|score_trees
specifier|static
name|int
name|score_trees
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|hash1
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|hash2
parameter_list|)
block|{
name|struct
name|tree_desc
name|one
decl_stmt|;
name|struct
name|tree_desc
name|two
decl_stmt|;
name|void
modifier|*
name|one_buf
init|=
name|fill_tree_desc_strict
argument_list|(
operator|&
name|one
argument_list|,
name|hash1
argument_list|)
decl_stmt|;
name|void
modifier|*
name|two_buf
init|=
name|fill_tree_desc_strict
argument_list|(
operator|&
name|two
argument_list|,
name|hash2
argument_list|)
decl_stmt|;
name|int
name|score
init|=
literal|0
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|struct
name|name_entry
name|e1
decl_stmt|,
name|e2
decl_stmt|;
name|int
name|got_entry_from_one
init|=
name|tree_entry
argument_list|(
operator|&
name|one
argument_list|,
operator|&
name|e1
argument_list|)
decl_stmt|;
name|int
name|got_entry_from_two
init|=
name|tree_entry
argument_list|(
operator|&
name|two
argument_list|,
operator|&
name|e2
argument_list|)
decl_stmt|;
name|int
name|cmp
decl_stmt|;
if|if
condition|(
name|got_entry_from_one
operator|&&
name|got_entry_from_two
condition|)
name|cmp
operator|=
name|base_name_entries_compare
argument_list|(
operator|&
name|e1
argument_list|,
operator|&
name|e2
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|got_entry_from_one
condition|)
comment|/* two lacks this entry */
name|cmp
operator|=
operator|-
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|got_entry_from_two
condition|)
comment|/* two has more entries */
name|cmp
operator|=
literal|1
expr_stmt|;
else|else
break|break;
if|if
condition|(
name|cmp
operator|<
literal|0
condition|)
comment|/* path1 does not appear in two */
name|score
operator|+=
name|score_missing
argument_list|(
name|e1
operator|.
name|mode
argument_list|,
name|e1
operator|.
name|path
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|cmp
operator|>
literal|0
condition|)
comment|/* path2 does not appear in one */
name|score
operator|+=
name|score_missing
argument_list|(
name|e2
operator|.
name|mode
argument_list|,
name|e2
operator|.
name|path
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|hashcmp
argument_list|(
name|e1
operator|.
name|sha1
argument_list|,
name|e2
operator|.
name|sha1
argument_list|)
condition|)
comment|/* they are different */
name|score
operator|+=
name|score_differs
argument_list|(
name|e1
operator|.
name|mode
argument_list|,
name|e2
operator|.
name|mode
argument_list|,
name|e1
operator|.
name|path
argument_list|)
expr_stmt|;
else|else
comment|/* same subtree or blob */
name|score
operator|+=
name|score_matches
argument_list|(
name|e1
operator|.
name|mode
argument_list|,
name|e2
operator|.
name|mode
argument_list|,
name|e1
operator|.
name|path
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|one_buf
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|two_buf
argument_list|)
expr_stmt|;
return|return
name|score
return|;
block|}
end_function
begin_comment
comment|/*  * Match one itself and its subtrees with two and pick the best match.  */
end_comment
begin_function
DECL|function|match_trees
specifier|static
name|void
name|match_trees
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|hash1
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|hash2
parameter_list|,
name|int
modifier|*
name|best_score
parameter_list|,
name|char
modifier|*
modifier|*
name|best_match
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|recurse_limit
parameter_list|)
block|{
name|struct
name|tree_desc
name|one
decl_stmt|;
name|void
modifier|*
name|one_buf
init|=
name|fill_tree_desc_strict
argument_list|(
operator|&
name|one
argument_list|,
name|hash1
argument_list|)
decl_stmt|;
while|while
condition|(
name|one
operator|.
name|size
condition|)
block|{
specifier|const
name|char
modifier|*
name|path
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|elem
decl_stmt|;
name|unsigned
name|mode
decl_stmt|;
name|int
name|score
decl_stmt|;
name|elem
operator|=
name|tree_entry_extract
argument_list|(
operator|&
name|one
argument_list|,
operator|&
name|path
argument_list|,
operator|&
name|mode
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|)
goto|goto
name|next
goto|;
name|score
operator|=
name|score_trees
argument_list|(
name|elem
argument_list|,
name|hash2
argument_list|)
expr_stmt|;
if|if
condition|(
operator|*
name|best_score
operator|<
name|score
condition|)
block|{
name|free
argument_list|(
operator|*
name|best_match
argument_list|)
expr_stmt|;
operator|*
name|best_match
operator|=
name|xstrfmt
argument_list|(
literal|"%s%s"
argument_list|,
name|base
argument_list|,
name|path
argument_list|)
expr_stmt|;
operator|*
name|best_score
operator|=
name|score
expr_stmt|;
block|}
if|if
condition|(
name|recurse_limit
condition|)
block|{
name|char
modifier|*
name|newbase
init|=
name|xstrfmt
argument_list|(
literal|"%s%s/"
argument_list|,
name|base
argument_list|,
name|path
argument_list|)
decl_stmt|;
name|match_trees
argument_list|(
name|elem
argument_list|,
name|hash2
argument_list|,
name|best_score
argument_list|,
name|best_match
argument_list|,
name|newbase
argument_list|,
name|recurse_limit
operator|-
literal|1
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|newbase
argument_list|)
expr_stmt|;
block|}
name|next
label|:
name|update_tree_entry
argument_list|(
operator|&
name|one
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|one_buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * A tree "hash1" has a subdirectory at "prefix".  Come up with a  * tree object by replacing it with another tree "hash2".  */
end_comment
begin_function
DECL|function|splice_tree
specifier|static
name|int
name|splice_tree
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|hash1
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|hash2
parameter_list|,
name|unsigned
name|char
modifier|*
name|result
parameter_list|)
block|{
name|char
modifier|*
name|subpath
decl_stmt|;
name|int
name|toplen
decl_stmt|;
name|char
modifier|*
name|buf
decl_stmt|;
name|unsigned
name|long
name|sz
decl_stmt|;
name|struct
name|tree_desc
name|desc
decl_stmt|;
name|unsigned
name|char
modifier|*
name|rewrite_here
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|rewrite_with
decl_stmt|;
name|unsigned
name|char
name|subtree
index|[
literal|20
index|]
decl_stmt|;
name|enum
name|object_type
name|type
decl_stmt|;
name|int
name|status
decl_stmt|;
name|subpath
operator|=
name|strchrnul
argument_list|(
name|prefix
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
name|toplen
operator|=
name|subpath
operator|-
name|prefix
expr_stmt|;
if|if
condition|(
operator|*
name|subpath
condition|)
name|subpath
operator|++
expr_stmt|;
name|buf
operator|=
name|read_sha1_file
argument_list|(
name|hash1
argument_list|,
operator|&
name|type
argument_list|,
operator|&
name|sz
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buf
condition|)
name|die
argument_list|(
literal|"cannot read tree %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|hash1
argument_list|)
argument_list|)
expr_stmt|;
name|init_tree_desc
argument_list|(
operator|&
name|desc
argument_list|,
name|buf
argument_list|,
name|sz
argument_list|)
expr_stmt|;
name|rewrite_here
operator|=
name|NULL
expr_stmt|;
while|while
condition|(
name|desc
operator|.
name|size
condition|)
block|{
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
name|unsigned
name|mode
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
decl_stmt|;
name|sha1
operator|=
name|tree_entry_extract
argument_list|(
operator|&
name|desc
argument_list|,
operator|&
name|name
argument_list|,
operator|&
name|mode
argument_list|)
expr_stmt|;
if|if
condition|(
name|strlen
argument_list|(
name|name
argument_list|)
operator|==
name|toplen
operator|&&
operator|!
name|memcmp
argument_list|(
name|name
argument_list|,
name|prefix
argument_list|,
name|toplen
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|)
name|die
argument_list|(
literal|"entry %s in tree %s is not a tree"
argument_list|,
name|name
argument_list|,
name|sha1_to_hex
argument_list|(
name|hash1
argument_list|)
argument_list|)
expr_stmt|;
name|rewrite_here
operator|=
operator|(
name|unsigned
name|char
operator|*
operator|)
name|sha1
expr_stmt|;
break|break;
block|}
name|update_tree_entry
argument_list|(
operator|&
name|desc
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|rewrite_here
condition|)
name|die
argument_list|(
literal|"entry %.*s not found in tree %s"
argument_list|,
name|toplen
argument_list|,
name|prefix
argument_list|,
name|sha1_to_hex
argument_list|(
name|hash1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|*
name|subpath
condition|)
block|{
name|status
operator|=
name|splice_tree
argument_list|(
name|rewrite_here
argument_list|,
name|subpath
argument_list|,
name|hash2
argument_list|,
name|subtree
argument_list|)
expr_stmt|;
if|if
condition|(
name|status
condition|)
return|return
name|status
return|;
name|rewrite_with
operator|=
name|subtree
expr_stmt|;
block|}
else|else
name|rewrite_with
operator|=
name|hash2
expr_stmt|;
name|hashcpy
argument_list|(
name|rewrite_here
argument_list|,
name|rewrite_with
argument_list|)
expr_stmt|;
name|status
operator|=
name|write_sha1_file
argument_list|(
name|buf
argument_list|,
name|sz
argument_list|,
name|tree_type
argument_list|,
name|result
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buf
argument_list|)
expr_stmt|;
return|return
name|status
return|;
block|}
end_function
begin_comment
comment|/*  * We are trying to come up with a merge between one and two that  * results in a tree shape similar to one.  The tree two might  * correspond to a subtree of one, in which case it needs to be  * shifted down by prefixing otherwise empty directories.  On the  * other hand, it could cover tree one and we might need to pick a  * subtree of it.  */
end_comment
begin_function
DECL|function|shift_tree
name|void
name|shift_tree
parameter_list|(
specifier|const
name|struct
name|object_id
modifier|*
name|hash1
parameter_list|,
specifier|const
name|struct
name|object_id
modifier|*
name|hash2
parameter_list|,
name|struct
name|object_id
modifier|*
name|shifted
parameter_list|,
name|int
name|depth_limit
parameter_list|)
block|{
name|char
modifier|*
name|add_prefix
decl_stmt|;
name|char
modifier|*
name|del_prefix
decl_stmt|;
name|int
name|add_score
decl_stmt|,
name|del_score
decl_stmt|;
comment|/* 	 * NEEDSWORK: this limits the recursion depth to hardcoded 	 * value '2' to avoid excessive overhead. 	 */
if|if
condition|(
operator|!
name|depth_limit
condition|)
name|depth_limit
operator|=
literal|2
expr_stmt|;
name|add_score
operator|=
name|del_score
operator|=
name|score_trees
argument_list|(
name|hash1
operator|->
name|hash
argument_list|,
name|hash2
operator|->
name|hash
argument_list|)
expr_stmt|;
name|add_prefix
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|del_prefix
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
literal|1
argument_list|)
expr_stmt|;
comment|/* 	 * See if one's subtree resembles two; if so we need to prefix 	 * two with a few fake trees to match the prefix. 	 */
name|match_trees
argument_list|(
name|hash1
operator|->
name|hash
argument_list|,
name|hash2
operator|->
name|hash
argument_list|,
operator|&
name|add_score
argument_list|,
operator|&
name|add_prefix
argument_list|,
literal|""
argument_list|,
name|depth_limit
argument_list|)
expr_stmt|;
comment|/* 	 * See if two's subtree resembles one; if so we need to 	 * pick only subtree of two. 	 */
name|match_trees
argument_list|(
name|hash2
operator|->
name|hash
argument_list|,
name|hash1
operator|->
name|hash
argument_list|,
operator|&
name|del_score
argument_list|,
operator|&
name|del_prefix
argument_list|,
literal|""
argument_list|,
name|depth_limit
argument_list|)
expr_stmt|;
comment|/* Assume we do not have to do any shifting */
name|oidcpy
argument_list|(
name|shifted
argument_list|,
name|hash2
argument_list|)
expr_stmt|;
if|if
condition|(
name|add_score
operator|<
name|del_score
condition|)
block|{
comment|/* We need to pick a subtree of two */
name|unsigned
name|mode
decl_stmt|;
if|if
condition|(
operator|!
operator|*
name|del_prefix
condition|)
return|return;
if|if
condition|(
name|get_tree_entry
argument_list|(
name|hash2
operator|->
name|hash
argument_list|,
name|del_prefix
argument_list|,
name|shifted
operator|->
name|hash
argument_list|,
operator|&
name|mode
argument_list|)
condition|)
name|die
argument_list|(
literal|"cannot find path %s in tree %s"
argument_list|,
name|del_prefix
argument_list|,
name|oid_to_hex
argument_list|(
name|hash2
argument_list|)
argument_list|)
expr_stmt|;
return|return;
block|}
if|if
condition|(
operator|!
operator|*
name|add_prefix
condition|)
return|return;
name|splice_tree
argument_list|(
name|hash1
operator|->
name|hash
argument_list|,
name|add_prefix
argument_list|,
name|hash2
operator|->
name|hash
argument_list|,
name|shifted
operator|->
name|hash
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * The user says the trees will be shifted by this much.  * Unfortunately we cannot fundamentally tell which one to  * be prefixed, as recursive merge can work in either direction.  */
end_comment
begin_function
DECL|function|shift_tree_by
name|void
name|shift_tree_by
parameter_list|(
specifier|const
name|struct
name|object_id
modifier|*
name|hash1
parameter_list|,
specifier|const
name|struct
name|object_id
modifier|*
name|hash2
parameter_list|,
name|struct
name|object_id
modifier|*
name|shifted
parameter_list|,
specifier|const
name|char
modifier|*
name|shift_prefix
parameter_list|)
block|{
name|struct
name|object_id
name|sub1
decl_stmt|,
name|sub2
decl_stmt|;
name|unsigned
name|mode1
decl_stmt|,
name|mode2
decl_stmt|;
name|unsigned
name|candidate
init|=
literal|0
decl_stmt|;
comment|/* Can hash2 be a tree at shift_prefix in tree hash1? */
if|if
condition|(
operator|!
name|get_tree_entry
argument_list|(
name|hash1
operator|->
name|hash
argument_list|,
name|shift_prefix
argument_list|,
name|sub1
operator|.
name|hash
argument_list|,
operator|&
name|mode1
argument_list|)
operator|&&
name|S_ISDIR
argument_list|(
name|mode1
argument_list|)
condition|)
name|candidate
operator||=
literal|1
expr_stmt|;
comment|/* Can hash1 be a tree at shift_prefix in tree hash2? */
if|if
condition|(
operator|!
name|get_tree_entry
argument_list|(
name|hash2
operator|->
name|hash
argument_list|,
name|shift_prefix
argument_list|,
name|sub2
operator|.
name|hash
argument_list|,
operator|&
name|mode2
argument_list|)
operator|&&
name|S_ISDIR
argument_list|(
name|mode2
argument_list|)
condition|)
name|candidate
operator||=
literal|2
expr_stmt|;
if|if
condition|(
name|candidate
operator|==
literal|3
condition|)
block|{
comment|/* Both are plausible -- we need to evaluate the score */
name|int
name|best_score
init|=
name|score_trees
argument_list|(
name|hash1
operator|->
name|hash
argument_list|,
name|hash2
operator|->
name|hash
argument_list|)
decl_stmt|;
name|int
name|score
decl_stmt|;
name|candidate
operator|=
literal|0
expr_stmt|;
name|score
operator|=
name|score_trees
argument_list|(
name|sub1
operator|.
name|hash
argument_list|,
name|hash2
operator|->
name|hash
argument_list|)
expr_stmt|;
if|if
condition|(
name|score
operator|>
name|best_score
condition|)
block|{
name|candidate
operator|=
literal|1
expr_stmt|;
name|best_score
operator|=
name|score
expr_stmt|;
block|}
name|score
operator|=
name|score_trees
argument_list|(
name|sub2
operator|.
name|hash
argument_list|,
name|hash1
operator|->
name|hash
argument_list|)
expr_stmt|;
if|if
condition|(
name|score
operator|>
name|best_score
condition|)
name|candidate
operator|=
literal|2
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|candidate
condition|)
block|{
comment|/* Neither is plausible -- do not shift */
name|oidcpy
argument_list|(
name|shifted
argument_list|,
name|hash2
argument_list|)
expr_stmt|;
return|return;
block|}
if|if
condition|(
name|candidate
operator|==
literal|1
condition|)
comment|/* 		 * shift tree2 down by adding shift_prefix above it 		 * to match tree1. 		 */
name|splice_tree
argument_list|(
name|hash1
operator|->
name|hash
argument_list|,
name|shift_prefix
argument_list|,
name|hash2
operator|->
name|hash
argument_list|,
name|shifted
operator|->
name|hash
argument_list|)
expr_stmt|;
else|else
comment|/* 		 * shift tree2 up by removing shift_prefix from it 		 * to match tree1. 		 */
name|oidcpy
argument_list|(
name|shifted
argument_list|,
operator|&
name|sub2
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
