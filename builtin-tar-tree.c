begin_unit
begin_comment
comment|/*  * Copyright (c) 2005, 2006 Rene Scharfe  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"tar.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"quote.h"
end_include
begin_decl_stmt
DECL|variable|tar_tree_usage
specifier|static
specifier|const
name|char
name|tar_tree_usage
index|[]
init|=
literal|"git-tar-tree [--remote=<repo>]<tree-ish> [basedir]\n"
literal|"*** Note that this command is now deprecated; use git-archive instead."
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cmd_tar_tree
name|int
name|cmd_tar_tree
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
comment|/* 	 * git-tar-tree is now a wrapper around git-archive --format=tar 	 * 	 * $0 --remote=<repo> arg... ==> 	 *	git-archive --format=tar --remote=<repo> arg... 	 * $0 tree-ish ==> 	 *	git-archive --format=tar tree-ish 	 * $0 tree-ish basedir ==> 	 * 	git-archive --format-tar --prefix=basedir tree-ish 	 */
name|int
name|i
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|nargv
init|=
name|xcalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|nargv
argument_list|)
argument_list|,
name|argc
operator|+
literal|2
argument_list|)
decl_stmt|;
name|char
modifier|*
name|basedir_arg
decl_stmt|;
name|int
name|nargc
init|=
literal|0
decl_stmt|;
name|nargv
index|[
name|nargc
operator|++
index|]
operator|=
literal|"git-archive"
expr_stmt|;
name|nargv
index|[
name|nargc
operator|++
index|]
operator|=
literal|"--format=tar"
expr_stmt|;
if|if
condition|(
literal|2
operator|<=
name|argc
operator|&&
operator|!
name|prefixcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--remote="
argument_list|)
condition|)
block|{
name|nargv
index|[
name|nargc
operator|++
index|]
operator|=
name|argv
index|[
literal|1
index|]
expr_stmt|;
name|argv
operator|++
expr_stmt|;
name|argc
operator|--
expr_stmt|;
block|}
switch|switch
condition|(
name|argc
condition|)
block|{
default|default:
name|usage
argument_list|(
name|tar_tree_usage
argument_list|)
expr_stmt|;
break|break;
case|case
literal|3
case|:
comment|/* base-path */
name|basedir_arg
operator|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|argv
index|[
literal|2
index|]
argument_list|)
operator|+
literal|11
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|basedir_arg
argument_list|,
literal|"--prefix=%s/"
argument_list|,
name|argv
index|[
literal|2
index|]
argument_list|)
expr_stmt|;
name|nargv
index|[
name|nargc
operator|++
index|]
operator|=
name|basedir_arg
expr_stmt|;
comment|/* fallthru */
case|case
literal|2
case|:
comment|/* tree-ish */
name|nargv
index|[
name|nargc
operator|++
index|]
operator|=
name|argv
index|[
literal|1
index|]
expr_stmt|;
block|}
name|nargv
index|[
name|nargc
index|]
operator|=
name|NULL
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"*** git-tar-tree is now deprecated.\n"
literal|"*** Running git-archive instead.\n***"
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
name|nargc
condition|;
name|i
operator|++
control|)
block|{
name|fputc
argument_list|(
literal|' '
argument_list|,
name|stderr
argument_list|)
expr_stmt|;
name|sq_quote_print
argument_list|(
name|stderr
argument_list|,
name|nargv
index|[
name|i
index|]
argument_list|)
expr_stmt|;
block|}
name|fputc
argument_list|(
literal|'\n'
argument_list|,
name|stderr
argument_list|)
expr_stmt|;
return|return
name|cmd_archive
argument_list|(
name|nargc
argument_list|,
name|nargv
argument_list|,
name|prefix
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/* ustar header + extended global header content */
end_comment
begin_define
DECL|macro|RECORDSIZE
define|#
directive|define
name|RECORDSIZE
value|(512)
end_define
begin_define
DECL|macro|HEADERSIZE
define|#
directive|define
name|HEADERSIZE
value|(2 * RECORDSIZE)
end_define
begin_function
DECL|function|cmd_get_tar_commit_id
name|int
name|cmd_get_tar_commit_id
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
name|char
name|buffer
index|[
name|HEADERSIZE
index|]
decl_stmt|;
name|struct
name|ustar_header
modifier|*
name|header
init|=
operator|(
expr|struct
name|ustar_header
operator|*
operator|)
name|buffer
decl_stmt|;
name|char
modifier|*
name|content
init|=
name|buffer
operator|+
name|RECORDSIZE
decl_stmt|;
name|ssize_t
name|n
decl_stmt|;
name|n
operator|=
name|read_in_full
argument_list|(
literal|0
argument_list|,
name|buffer
argument_list|,
name|HEADERSIZE
argument_list|)
expr_stmt|;
if|if
condition|(
name|n
operator|<
name|HEADERSIZE
condition|)
name|die
argument_list|(
literal|"git-get-tar-commit-id: read error"
argument_list|)
expr_stmt|;
if|if
condition|(
name|header
operator|->
name|typeflag
index|[
literal|0
index|]
operator|!=
literal|'g'
condition|)
return|return
literal|1
return|;
if|if
condition|(
name|memcmp
argument_list|(
name|content
argument_list|,
literal|"52 comment="
argument_list|,
literal|11
argument_list|)
condition|)
return|return
literal|1
return|;
name|n
operator|=
name|write_in_full
argument_list|(
literal|1
argument_list|,
name|content
operator|+
literal|11
argument_list|,
literal|41
argument_list|)
expr_stmt|;
if|if
condition|(
name|n
operator|<
literal|41
condition|)
name|die
argument_list|(
literal|"git-get-tar-commit-id: write error"
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
