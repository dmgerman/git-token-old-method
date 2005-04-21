begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_decl_stmt
DECL|variable|recursive
specifier|static
name|int
name|recursive
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|line_termination
specifier|static
name|int
name|line_termination
init|=
literal|'\n'
decl_stmt|;
end_decl_stmt
begin_function_decl
specifier|static
name|int
name|diff_tree_sha1
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|old
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|new
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|update_tree_entry
specifier|static
name|void
name|update_tree_entry
parameter_list|(
name|void
modifier|*
modifier|*
name|bufp
parameter_list|,
name|unsigned
name|long
modifier|*
name|sizep
parameter_list|)
block|{
name|void
modifier|*
name|buf
init|=
operator|*
name|bufp
decl_stmt|;
name|unsigned
name|long
name|size
init|=
operator|*
name|sizep
decl_stmt|;
name|int
name|len
init|=
name|strlen
argument_list|(
name|buf
argument_list|)
operator|+
literal|1
operator|+
literal|20
decl_stmt|;
if|if
condition|(
name|size
operator|<
name|len
condition|)
name|die
argument_list|(
literal|"corrupt tree file"
argument_list|)
expr_stmt|;
operator|*
name|bufp
operator|=
name|buf
operator|+
name|len
expr_stmt|;
operator|*
name|sizep
operator|=
name|size
operator|-
name|len
expr_stmt|;
block|}
end_function
begin_function
DECL|function|extract
specifier|static
specifier|const
name|unsigned
name|char
modifier|*
name|extract
parameter_list|(
name|void
modifier|*
name|tree
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|pathp
parameter_list|,
name|unsigned
name|int
modifier|*
name|modep
parameter_list|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|tree
argument_list|)
operator|+
literal|1
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
init|=
name|tree
operator|+
name|len
decl_stmt|;
specifier|const
name|char
modifier|*
name|path
init|=
name|strchr
argument_list|(
name|tree
argument_list|,
literal|' '
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|path
operator|||
name|size
operator|<
name|len
operator|+
literal|20
operator|||
name|sscanf
argument_list|(
name|tree
argument_list|,
literal|"%o"
argument_list|,
name|modep
argument_list|)
operator|!=
literal|1
condition|)
name|die
argument_list|(
literal|"corrupt tree file"
argument_list|)
expr_stmt|;
operator|*
name|pathp
operator|=
name|path
operator|+
literal|1
expr_stmt|;
return|return
name|sha1
return|;
block|}
end_function
begin_function
DECL|function|malloc_base
specifier|static
name|char
modifier|*
name|malloc_base
parameter_list|(
specifier|const
name|char
modifier|*
name|base
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|pathlen
parameter_list|)
block|{
name|int
name|baselen
init|=
name|strlen
argument_list|(
name|base
argument_list|)
decl_stmt|;
name|char
modifier|*
name|newbase
init|=
name|malloc
argument_list|(
name|baselen
operator|+
name|pathlen
operator|+
literal|2
argument_list|)
decl_stmt|;
name|memcpy
argument_list|(
name|newbase
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|newbase
operator|+
name|baselen
argument_list|,
name|path
argument_list|,
name|pathlen
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|newbase
operator|+
name|baselen
operator|+
name|pathlen
argument_list|,
literal|"/"
argument_list|,
literal|2
argument_list|)
expr_stmt|;
return|return
name|newbase
return|;
block|}
end_function
begin_function_decl
specifier|static
name|void
name|show_file
parameter_list|(
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|void
modifier|*
name|tree
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* A whole sub-tree went away or appeared */
end_comment
begin_function
DECL|function|show_tree
specifier|static
name|void
name|show_tree
parameter_list|(
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|void
modifier|*
name|tree
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
block|{
while|while
condition|(
name|size
condition|)
block|{
name|show_file
argument_list|(
name|prefix
argument_list|,
name|tree
argument_list|,
name|size
argument_list|,
name|base
argument_list|)
expr_stmt|;
name|update_tree_entry
argument_list|(
operator|&
name|tree
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/* A file entry went away or appeared */
end_comment
begin_function
DECL|function|show_file
specifier|static
name|void
name|show_file
parameter_list|(
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|void
modifier|*
name|tree
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
block|{
name|unsigned
name|mode
decl_stmt|;
specifier|const
name|char
modifier|*
name|path
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
init|=
name|extract
argument_list|(
name|tree
argument_list|,
name|size
argument_list|,
operator|&
name|path
argument_list|,
operator|&
name|mode
argument_list|)
decl_stmt|;
if|if
condition|(
name|recursive
operator|&&
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|)
block|{
name|char
name|type
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|char
modifier|*
name|newbase
init|=
name|malloc_base
argument_list|(
name|base
argument_list|,
name|path
argument_list|,
name|strlen
argument_list|(
name|path
argument_list|)
argument_list|)
decl_stmt|;
name|void
modifier|*
name|tree
decl_stmt|;
name|tree
operator|=
name|read_sha1_file
argument_list|(
name|sha1
argument_list|,
name|type
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree
operator|||
name|strcmp
argument_list|(
name|type
argument_list|,
literal|"tree"
argument_list|)
condition|)
name|die
argument_list|(
literal|"corrupt tree sha %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|show_tree
argument_list|(
name|prefix
argument_list|,
name|tree
argument_list|,
name|size
argument_list|,
name|newbase
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|tree
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|newbase
argument_list|)
expr_stmt|;
return|return;
block|}
name|printf
argument_list|(
literal|"%s%o\t%s\t%s\t%s%s%c"
argument_list|,
name|prefix
argument_list|,
name|mode
argument_list|,
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|?
literal|"tree"
else|:
literal|"blob"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|base
argument_list|,
name|path
argument_list|,
name|line_termination
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|compare_tree_entry
specifier|static
name|int
name|compare_tree_entry
parameter_list|(
name|void
modifier|*
name|tree1
parameter_list|,
name|unsigned
name|long
name|size1
parameter_list|,
name|void
modifier|*
name|tree2
parameter_list|,
name|unsigned
name|long
name|size2
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
block|{
name|unsigned
name|mode1
decl_stmt|,
name|mode2
decl_stmt|;
specifier|const
name|char
modifier|*
name|path1
decl_stmt|,
modifier|*
name|path2
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
decl_stmt|,
modifier|*
name|sha2
decl_stmt|;
name|int
name|cmp
decl_stmt|,
name|pathlen1
decl_stmt|,
name|pathlen2
decl_stmt|;
name|char
name|old_sha1_hex
index|[
literal|50
index|]
decl_stmt|;
name|sha1
operator|=
name|extract
argument_list|(
name|tree1
argument_list|,
name|size1
argument_list|,
operator|&
name|path1
argument_list|,
operator|&
name|mode1
argument_list|)
expr_stmt|;
name|sha2
operator|=
name|extract
argument_list|(
name|tree2
argument_list|,
name|size2
argument_list|,
operator|&
name|path2
argument_list|,
operator|&
name|mode2
argument_list|)
expr_stmt|;
name|pathlen1
operator|=
name|strlen
argument_list|(
name|path1
argument_list|)
expr_stmt|;
name|pathlen2
operator|=
name|strlen
argument_list|(
name|path2
argument_list|)
expr_stmt|;
name|cmp
operator|=
name|cache_name_compare
argument_list|(
name|path1
argument_list|,
name|pathlen1
argument_list|,
name|path2
argument_list|,
name|pathlen2
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmp
operator|<
literal|0
condition|)
block|{
name|show_file
argument_list|(
literal|"-"
argument_list|,
name|tree1
argument_list|,
name|size1
argument_list|,
name|base
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|cmp
operator|>
literal|0
condition|)
block|{
name|show_file
argument_list|(
literal|"+"
argument_list|,
name|tree2
argument_list|,
name|size2
argument_list|,
name|base
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|sha1
argument_list|,
name|sha2
argument_list|,
literal|20
argument_list|)
operator|&&
name|mode1
operator|==
name|mode2
condition|)
return|return
literal|0
return|;
comment|/* 	 * If the filemode has changed to/from a directory from/to a regular 	 * file, we need to consider it a remove and an add. 	 */
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
block|{
name|show_file
argument_list|(
literal|"-"
argument_list|,
name|tree1
argument_list|,
name|size1
argument_list|,
name|base
argument_list|)
expr_stmt|;
name|show_file
argument_list|(
literal|"+"
argument_list|,
name|tree2
argument_list|,
name|size2
argument_list|,
name|base
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|recursive
operator|&&
name|S_ISDIR
argument_list|(
name|mode1
argument_list|)
condition|)
block|{
name|int
name|retval
decl_stmt|;
name|char
modifier|*
name|newbase
init|=
name|malloc_base
argument_list|(
name|base
argument_list|,
name|path1
argument_list|,
name|pathlen1
argument_list|)
decl_stmt|;
name|retval
operator|=
name|diff_tree_sha1
argument_list|(
name|sha1
argument_list|,
name|sha2
argument_list|,
name|newbase
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|newbase
argument_list|)
expr_stmt|;
return|return
name|retval
return|;
block|}
name|strcpy
argument_list|(
name|old_sha1_hex
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"*%o->%o\t%s\t%s->%s\t%s%s%c"
argument_list|,
name|mode1
argument_list|,
name|mode2
argument_list|,
name|S_ISDIR
argument_list|(
name|mode1
argument_list|)
condition|?
literal|"tree"
else|:
literal|"blob"
argument_list|,
name|old_sha1_hex
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha2
argument_list|)
argument_list|,
name|base
argument_list|,
name|path1
argument_list|,
name|line_termination
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|diff_tree
specifier|static
name|int
name|diff_tree
parameter_list|(
name|void
modifier|*
name|tree1
parameter_list|,
name|unsigned
name|long
name|size1
parameter_list|,
name|void
modifier|*
name|tree2
parameter_list|,
name|unsigned
name|long
name|size2
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
block|{
while|while
condition|(
name|size1
operator||
name|size2
condition|)
block|{
if|if
condition|(
operator|!
name|size1
condition|)
block|{
name|show_file
argument_list|(
literal|"+"
argument_list|,
name|tree2
argument_list|,
name|size2
argument_list|,
name|base
argument_list|)
expr_stmt|;
name|update_tree_entry
argument_list|(
operator|&
name|tree2
argument_list|,
operator|&
name|size2
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|size2
condition|)
block|{
name|show_file
argument_list|(
literal|"-"
argument_list|,
name|tree1
argument_list|,
name|size1
argument_list|,
name|base
argument_list|)
expr_stmt|;
name|update_tree_entry
argument_list|(
operator|&
name|tree1
argument_list|,
operator|&
name|size1
argument_list|)
expr_stmt|;
continue|continue;
block|}
switch|switch
condition|(
name|compare_tree_entry
argument_list|(
name|tree1
argument_list|,
name|size1
argument_list|,
name|tree2
argument_list|,
name|size2
argument_list|,
name|base
argument_list|)
condition|)
block|{
case|case
operator|-
literal|1
case|:
name|update_tree_entry
argument_list|(
operator|&
name|tree1
argument_list|,
operator|&
name|size1
argument_list|)
expr_stmt|;
continue|continue;
case|case
literal|0
case|:
name|update_tree_entry
argument_list|(
operator|&
name|tree1
argument_list|,
operator|&
name|size1
argument_list|)
expr_stmt|;
comment|/* Fallthrough */
case|case
literal|1
case|:
name|update_tree_entry
argument_list|(
operator|&
name|tree2
argument_list|,
operator|&
name|size2
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|die
argument_list|(
literal|"diff-tree: internal error"
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|diff_tree_sha1
specifier|static
name|int
name|diff_tree_sha1
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|old
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|new
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
block|{
name|void
modifier|*
name|tree1
decl_stmt|,
modifier|*
name|tree2
decl_stmt|;
name|unsigned
name|long
name|size1
decl_stmt|,
name|size2
decl_stmt|;
name|int
name|retval
decl_stmt|;
name|tree1
operator|=
name|read_tree_with_tree_or_commit_sha1
argument_list|(
name|old
argument_list|,
operator|&
name|size1
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree1
condition|)
name|die
argument_list|(
literal|"unable to read source tree (%s)"
argument_list|,
name|sha1_to_hex
argument_list|(
name|old
argument_list|)
argument_list|)
expr_stmt|;
name|tree2
operator|=
name|read_tree_with_tree_or_commit_sha1
argument_list|(
name|new
argument_list|,
operator|&
name|size2
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree2
condition|)
name|die
argument_list|(
literal|"unable to read destination tree (%s)"
argument_list|,
name|sha1_to_hex
argument_list|(
name|new
argument_list|)
argument_list|)
expr_stmt|;
name|retval
operator|=
name|diff_tree
argument_list|(
name|tree1
argument_list|,
name|size1
argument_list|,
name|tree2
argument_list|,
name|size2
argument_list|,
name|base
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|tree1
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|tree2
argument_list|)
expr_stmt|;
return|return
name|retval
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|diff_tree_usage
specifier|static
name|char
modifier|*
name|diff_tree_usage
init|=
literal|"diff-tree [-r] [-z]<tree sha1><tree sha1>"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|unsigned
name|char
name|old
index|[
literal|20
index|]
decl_stmt|,
name|new
index|[
literal|20
index|]
decl_stmt|;
while|while
condition|(
name|argc
operator|>
literal|3
condition|)
block|{
name|char
modifier|*
name|arg
init|=
name|argv
index|[
literal|1
index|]
decl_stmt|;
name|argv
operator|++
expr_stmt|;
name|argc
operator|--
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-r"
argument_list|)
condition|)
block|{
name|recursive
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-z"
argument_list|)
condition|)
block|{
name|line_termination
operator|=
literal|'\0'
expr_stmt|;
continue|continue;
block|}
name|usage
argument_list|(
name|diff_tree_usage
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|argc
operator|!=
literal|3
operator|||
name|get_sha1_hex
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
name|old
argument_list|)
operator|||
name|get_sha1_hex
argument_list|(
name|argv
index|[
literal|2
index|]
argument_list|,
name|new
argument_list|)
condition|)
name|usage
argument_list|(
name|diff_tree_usage
argument_list|)
expr_stmt|;
return|return
name|diff_tree_sha1
argument_list|(
name|old
argument_list|,
name|new
argument_list|,
literal|""
argument_list|)
return|;
block|}
end_function
end_unit
