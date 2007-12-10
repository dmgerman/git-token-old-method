begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"dir.h"
end_include
begin_decl_stmt
DECL|variable|inside_git_dir
specifier|static
name|int
name|inside_git_dir
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|inside_work_tree
specifier|static
name|int
name|inside_work_tree
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|prefix_path
specifier|const
name|char
modifier|*
name|prefix_path
parameter_list|(
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|int
name|len
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|orig
init|=
name|path
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|char
name|c
decl_stmt|;
if|if
condition|(
operator|*
name|path
operator|!=
literal|'.'
condition|)
break|break;
name|c
operator|=
name|path
index|[
literal|1
index|]
expr_stmt|;
comment|/* "." */
if|if
condition|(
operator|!
name|c
condition|)
block|{
name|path
operator|++
expr_stmt|;
break|break;
block|}
comment|/* "./" */
if|if
condition|(
name|c
operator|==
literal|'/'
condition|)
block|{
name|path
operator|+=
literal|2
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|c
operator|!=
literal|'.'
condition|)
break|break;
name|c
operator|=
name|path
index|[
literal|2
index|]
expr_stmt|;
if|if
condition|(
operator|!
name|c
condition|)
name|path
operator|+=
literal|2
expr_stmt|;
elseif|else
if|if
condition|(
name|c
operator|==
literal|'/'
condition|)
name|path
operator|+=
literal|3
expr_stmt|;
else|else
break|break;
comment|/* ".." and "../" */
comment|/* Remove last component of the prefix */
do|do
block|{
if|if
condition|(
operator|!
name|len
condition|)
name|die
argument_list|(
literal|"'%s' is outside repository"
argument_list|,
name|orig
argument_list|)
expr_stmt|;
name|len
operator|--
expr_stmt|;
block|}
do|while
condition|(
name|len
operator|&&
name|prefix
index|[
name|len
operator|-
literal|1
index|]
operator|!=
literal|'/'
condition|)
do|;
continue|continue;
block|}
if|if
condition|(
name|len
condition|)
block|{
name|int
name|speclen
init|=
name|strlen
argument_list|(
name|path
argument_list|)
decl_stmt|;
name|char
modifier|*
name|n
init|=
name|xmalloc
argument_list|(
name|speclen
operator|+
name|len
operator|+
literal|1
argument_list|)
decl_stmt|;
name|memcpy
argument_list|(
name|n
argument_list|,
name|prefix
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|n
operator|+
name|len
argument_list|,
name|path
argument_list|,
name|speclen
operator|+
literal|1
argument_list|)
expr_stmt|;
name|path
operator|=
name|n
expr_stmt|;
block|}
return|return
name|path
return|;
block|}
end_function
begin_comment
comment|/*  * Unlike prefix_path, this should be used if the named file does  * not have to interact with index entry; i.e. name of a random file  * on the filesystem.  */
end_comment
begin_function
DECL|function|prefix_filename
specifier|const
name|char
modifier|*
name|prefix_filename
parameter_list|(
specifier|const
name|char
modifier|*
name|pfx
parameter_list|,
name|int
name|pfx_len
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|)
block|{
specifier|static
name|char
name|path
index|[
name|PATH_MAX
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|pfx
operator|||
operator|!
operator|*
name|pfx
operator|||
name|is_absolute_path
argument_list|(
name|arg
argument_list|)
condition|)
return|return
name|arg
return|;
name|memcpy
argument_list|(
name|path
argument_list|,
name|pfx
argument_list|,
name|pfx_len
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|path
operator|+
name|pfx_len
argument_list|,
name|arg
argument_list|)
expr_stmt|;
return|return
name|path
return|;
block|}
end_function
begin_comment
comment|/*  * Verify a filename that we got as an argument for a pathspec  * entry. Note that a filename that begins with "-" never verifies  * as true, because even if such a filename were to exist, we want  * it to be preceded by the "--" marker (or we want the user to  * use a format like "./-filename")  */
end_comment
begin_function
DECL|function|verify_filename
name|void
name|verify_filename
parameter_list|(
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
operator|*
name|arg
operator|==
literal|'-'
condition|)
name|die
argument_list|(
literal|"bad flag '%s' used after filename"
argument_list|,
name|arg
argument_list|)
expr_stmt|;
name|name
operator|=
name|prefix
condition|?
name|prefix_filename
argument_list|(
name|prefix
argument_list|,
name|strlen
argument_list|(
name|prefix
argument_list|)
argument_list|,
name|arg
argument_list|)
else|:
name|arg
expr_stmt|;
if|if
condition|(
operator|!
name|lstat
argument_list|(
name|name
argument_list|,
operator|&
name|st
argument_list|)
condition|)
return|return;
if|if
condition|(
name|errno
operator|==
name|ENOENT
condition|)
name|die
argument_list|(
literal|"ambiguous argument '%s': unknown revision or path not in the working tree.\n"
literal|"Use '--' to separate paths from revisions"
argument_list|,
name|arg
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"'%s': %s"
argument_list|,
name|arg
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * Opposite of the above: the command line did not have -- marker  * and we parsed the arg as a refname.  It should not be interpretable  * as a filename.  */
end_comment
begin_function
DECL|function|verify_non_filename
name|void
name|verify_non_filename
parameter_list|(
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
operator|!
name|is_inside_work_tree
argument_list|()
operator|||
name|is_inside_git_dir
argument_list|()
condition|)
return|return;
if|if
condition|(
operator|*
name|arg
operator|==
literal|'-'
condition|)
return|return;
comment|/* flag */
name|name
operator|=
name|prefix
condition|?
name|prefix_filename
argument_list|(
name|prefix
argument_list|,
name|strlen
argument_list|(
name|prefix
argument_list|)
argument_list|,
name|arg
argument_list|)
else|:
name|arg
expr_stmt|;
if|if
condition|(
operator|!
name|lstat
argument_list|(
name|name
argument_list|,
operator|&
name|st
argument_list|)
condition|)
name|die
argument_list|(
literal|"ambiguous argument '%s': both revision and filename\n"
literal|"Use '--' to separate filenames from revisions"
argument_list|,
name|arg
argument_list|)
expr_stmt|;
if|if
condition|(
name|errno
operator|!=
name|ENOENT
operator|&&
name|errno
operator|!=
name|ENOTDIR
condition|)
name|die
argument_list|(
literal|"'%s': %s"
argument_list|,
name|arg
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_pathspec
specifier|const
name|char
modifier|*
modifier|*
name|get_pathspec
parameter_list|(
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|entry
init|=
operator|*
name|pathspec
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|p
decl_stmt|;
name|int
name|prefixlen
decl_stmt|;
if|if
condition|(
operator|!
name|prefix
operator|&&
operator|!
name|entry
condition|)
return|return
name|NULL
return|;
if|if
condition|(
operator|!
name|entry
condition|)
block|{
specifier|static
specifier|const
name|char
modifier|*
name|spec
index|[
literal|2
index|]
decl_stmt|;
name|spec
index|[
literal|0
index|]
operator|=
name|prefix
expr_stmt|;
name|spec
index|[
literal|1
index|]
operator|=
name|NULL
expr_stmt|;
return|return
name|spec
return|;
block|}
comment|/* Otherwise we have to re-write the entries.. */
name|p
operator|=
name|pathspec
expr_stmt|;
name|prefixlen
operator|=
name|prefix
condition|?
name|strlen
argument_list|(
name|prefix
argument_list|)
else|:
literal|0
expr_stmt|;
do|do
block|{
operator|*
name|p
operator|=
name|prefix_path
argument_list|(
name|prefix
argument_list|,
name|prefixlen
argument_list|,
name|entry
argument_list|)
expr_stmt|;
block|}
do|while
condition|(
operator|(
name|entry
operator|=
operator|*
operator|++
name|p
operator|)
operator|!=
name|NULL
condition|)
do|;
return|return
operator|(
specifier|const
name|char
operator|*
operator|*
operator|)
name|pathspec
return|;
block|}
end_function
begin_comment
comment|/*  * Test if it looks like we're at a git directory.  * We want to see:  *  *  - either a objects/ directory _or_ the proper  *    GIT_OBJECT_DIRECTORY environment variable  *  - a refs/ directory  *  - either a HEAD symlink or a HEAD file that is formatted as  *    a proper "ref:", or a regular file HEAD that has a properly  *    formatted sha1 object name.  */
end_comment
begin_function
DECL|function|is_git_directory
specifier|static
name|int
name|is_git_directory
parameter_list|(
specifier|const
name|char
modifier|*
name|suspect
parameter_list|)
block|{
name|char
name|path
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|size_t
name|len
init|=
name|strlen
argument_list|(
name|suspect
argument_list|)
decl_stmt|;
name|strcpy
argument_list|(
name|path
argument_list|,
name|suspect
argument_list|)
expr_stmt|;
if|if
condition|(
name|getenv
argument_list|(
name|DB_ENVIRONMENT
argument_list|)
condition|)
block|{
if|if
condition|(
name|access
argument_list|(
name|getenv
argument_list|(
name|DB_ENVIRONMENT
argument_list|)
argument_list|,
name|X_OK
argument_list|)
condition|)
return|return
literal|0
return|;
block|}
else|else
block|{
name|strcpy
argument_list|(
name|path
operator|+
name|len
argument_list|,
literal|"/objects"
argument_list|)
expr_stmt|;
if|if
condition|(
name|access
argument_list|(
name|path
argument_list|,
name|X_OK
argument_list|)
condition|)
return|return
literal|0
return|;
block|}
name|strcpy
argument_list|(
name|path
operator|+
name|len
argument_list|,
literal|"/refs"
argument_list|)
expr_stmt|;
if|if
condition|(
name|access
argument_list|(
name|path
argument_list|,
name|X_OK
argument_list|)
condition|)
return|return
literal|0
return|;
name|strcpy
argument_list|(
name|path
operator|+
name|len
argument_list|,
literal|"/HEAD"
argument_list|)
expr_stmt|;
if|if
condition|(
name|validate_headref
argument_list|(
name|path
argument_list|)
condition|)
return|return
literal|0
return|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|is_inside_git_dir
name|int
name|is_inside_git_dir
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
name|inside_git_dir
operator|<
literal|0
condition|)
name|inside_git_dir
operator|=
name|is_inside_dir
argument_list|(
name|get_git_dir
argument_list|()
argument_list|)
expr_stmt|;
return|return
name|inside_git_dir
return|;
block|}
end_function
begin_function
DECL|function|is_inside_work_tree
name|int
name|is_inside_work_tree
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
name|inside_work_tree
operator|<
literal|0
condition|)
name|inside_work_tree
operator|=
name|is_inside_dir
argument_list|(
name|get_git_work_tree
argument_list|()
argument_list|)
expr_stmt|;
return|return
name|inside_work_tree
return|;
block|}
end_function
begin_comment
comment|/*  * set_work_tree() is only ever called if you set GIT_DIR explicitely.  * The old behaviour (which we retain here) is to set the work tree root  * to the cwd, unless overridden by the config, the command line, or  * GIT_WORK_TREE.  */
end_comment
begin_function
DECL|function|set_work_tree
specifier|static
specifier|const
name|char
modifier|*
name|set_work_tree
parameter_list|(
specifier|const
name|char
modifier|*
name|dir
parameter_list|)
block|{
name|char
name|buffer
index|[
name|PATH_MAX
operator|+
literal|1
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|getcwd
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|)
condition|)
name|die
argument_list|(
literal|"Could not get the current working directory"
argument_list|)
expr_stmt|;
name|git_work_tree_cfg
operator|=
name|xstrdup
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
name|inside_work_tree
operator|=
literal|1
expr_stmt|;
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|setup_work_tree
name|void
name|setup_work_tree
parameter_list|(
name|void
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|work_tree
decl_stmt|,
modifier|*
name|git_dir
decl_stmt|;
specifier|static
name|int
name|initialized
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|initialized
condition|)
return|return;
name|work_tree
operator|=
name|get_git_work_tree
argument_list|()
expr_stmt|;
name|git_dir
operator|=
name|get_git_dir
argument_list|()
expr_stmt|;
if|if
condition|(
operator|!
name|is_absolute_path
argument_list|(
name|git_dir
argument_list|)
condition|)
name|set_git_dir
argument_list|(
name|make_absolute_path
argument_list|(
name|git_dir
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|work_tree
operator|||
name|chdir
argument_list|(
name|work_tree
argument_list|)
condition|)
name|die
argument_list|(
literal|"This operation must be run in a work tree"
argument_list|)
expr_stmt|;
name|initialized
operator|=
literal|1
expr_stmt|;
block|}
end_function
begin_function
DECL|function|check_repository_format_gently
specifier|static
name|int
name|check_repository_format_gently
parameter_list|(
name|int
modifier|*
name|nongit_ok
parameter_list|)
block|{
name|git_config
argument_list|(
name|check_repository_format_version
argument_list|)
expr_stmt|;
if|if
condition|(
name|GIT_REPO_VERSION
operator|<
name|repository_format_version
condition|)
block|{
if|if
condition|(
operator|!
name|nongit_ok
condition|)
name|die
argument_list|(
literal|"Expected git repo version<= %d, found %d"
argument_list|,
name|GIT_REPO_VERSION
argument_list|,
name|repository_format_version
argument_list|)
expr_stmt|;
name|warning
argument_list|(
literal|"Expected git repo version<= %d, found %d"
argument_list|,
name|GIT_REPO_VERSION
argument_list|,
name|repository_format_version
argument_list|)
expr_stmt|;
name|warning
argument_list|(
literal|"Please upgrade Git"
argument_list|)
expr_stmt|;
operator|*
name|nongit_ok
operator|=
operator|-
literal|1
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/*  * We cannot decide in this function whether we are in the work tree or  * not, since the config can only be read _after_ this function was called.  */
end_comment
begin_function
DECL|function|setup_git_directory_gently
specifier|const
name|char
modifier|*
name|setup_git_directory_gently
parameter_list|(
name|int
modifier|*
name|nongit_ok
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|work_tree_env
init|=
name|getenv
argument_list|(
name|GIT_WORK_TREE_ENVIRONMENT
argument_list|)
decl_stmt|;
specifier|static
name|char
name|cwd
index|[
name|PATH_MAX
operator|+
literal|1
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|gitdirenv
decl_stmt|;
name|int
name|len
decl_stmt|,
name|offset
decl_stmt|;
comment|/* 	 * If GIT_DIR is set explicitly, we're not going 	 * to do any discovery, but we still do repository 	 * validation. 	 */
name|gitdirenv
operator|=
name|getenv
argument_list|(
name|GIT_DIR_ENVIRONMENT
argument_list|)
expr_stmt|;
if|if
condition|(
name|gitdirenv
condition|)
block|{
if|if
condition|(
name|PATH_MAX
operator|-
literal|40
operator|<
name|strlen
argument_list|(
name|gitdirenv
argument_list|)
condition|)
name|die
argument_list|(
literal|"'$%s' too big"
argument_list|,
name|GIT_DIR_ENVIRONMENT
argument_list|)
expr_stmt|;
if|if
condition|(
name|is_git_directory
argument_list|(
name|gitdirenv
argument_list|)
condition|)
block|{
specifier|static
name|char
name|buffer
index|[
literal|1024
operator|+
literal|1
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|retval
decl_stmt|;
if|if
condition|(
operator|!
name|work_tree_env
condition|)
block|{
name|retval
operator|=
name|set_work_tree
argument_list|(
name|gitdirenv
argument_list|)
expr_stmt|;
comment|/* config may override worktree */
if|if
condition|(
name|check_repository_format_gently
argument_list|(
name|nongit_ok
argument_list|)
condition|)
return|return
name|NULL
return|;
return|return
name|retval
return|;
block|}
if|if
condition|(
name|check_repository_format_gently
argument_list|(
name|nongit_ok
argument_list|)
condition|)
return|return
name|NULL
return|;
name|retval
operator|=
name|get_relative_cwd
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
operator|-
literal|1
argument_list|,
name|get_git_work_tree
argument_list|()
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|retval
operator|||
operator|!
operator|*
name|retval
condition|)
return|return
name|NULL
return|;
name|set_git_dir
argument_list|(
name|make_absolute_path
argument_list|(
name|gitdirenv
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|chdir
argument_list|(
name|work_tree_env
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"Could not chdir to %s"
argument_list|,
name|work_tree_env
argument_list|)
expr_stmt|;
name|strcat
argument_list|(
name|buffer
argument_list|,
literal|"/"
argument_list|)
expr_stmt|;
return|return
name|retval
return|;
block|}
if|if
condition|(
name|nongit_ok
condition|)
block|{
operator|*
name|nongit_ok
operator|=
literal|1
expr_stmt|;
return|return
name|NULL
return|;
block|}
name|die
argument_list|(
literal|"Not a git repository: '%s'"
argument_list|,
name|gitdirenv
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|getcwd
argument_list|(
name|cwd
argument_list|,
sizeof|sizeof
argument_list|(
name|cwd
argument_list|)
operator|-
literal|1
argument_list|)
condition|)
name|die
argument_list|(
literal|"Unable to read current working directory"
argument_list|)
expr_stmt|;
comment|/* 	 * Test in the following order (relative to the cwd): 	 * - .git/ 	 * - ./ (bare) 	 * - ../.git/ 	 * - ../ (bare) 	 * - ../../.git/ 	 *   etc. 	 */
name|offset
operator|=
name|len
operator|=
name|strlen
argument_list|(
name|cwd
argument_list|)
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
if|if
condition|(
name|is_git_directory
argument_list|(
name|DEFAULT_GIT_DIR_ENVIRONMENT
argument_list|)
condition|)
break|break;
if|if
condition|(
name|is_git_directory
argument_list|(
literal|"."
argument_list|)
condition|)
block|{
name|inside_git_dir
operator|=
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|work_tree_env
condition|)
name|inside_work_tree
operator|=
literal|0
expr_stmt|;
name|setenv
argument_list|(
name|GIT_DIR_ENVIRONMENT
argument_list|,
literal|"."
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|check_repository_format_gently
argument_list|(
name|nongit_ok
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
name|chdir
argument_list|(
literal|".."
argument_list|)
expr_stmt|;
do|do
block|{
if|if
condition|(
operator|!
name|offset
condition|)
block|{
if|if
condition|(
name|nongit_ok
condition|)
block|{
if|if
condition|(
name|chdir
argument_list|(
name|cwd
argument_list|)
condition|)
name|die
argument_list|(
literal|"Cannot come back to cwd"
argument_list|)
expr_stmt|;
operator|*
name|nongit_ok
operator|=
literal|1
expr_stmt|;
return|return
name|NULL
return|;
block|}
name|die
argument_list|(
literal|"Not a git repository"
argument_list|)
expr_stmt|;
block|}
block|}
do|while
condition|(
name|cwd
index|[
operator|--
name|offset
index|]
operator|!=
literal|'/'
condition|)
do|;
block|}
name|inside_git_dir
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|work_tree_env
condition|)
name|inside_work_tree
operator|=
literal|1
expr_stmt|;
name|git_work_tree_cfg
operator|=
name|xstrndup
argument_list|(
name|cwd
argument_list|,
name|offset
argument_list|)
expr_stmt|;
if|if
condition|(
name|check_repository_format_gently
argument_list|(
name|nongit_ok
argument_list|)
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|offset
operator|==
name|len
condition|)
return|return
name|NULL
return|;
comment|/* Make "offset" point to past the '/', and add a '/' at the end */
name|offset
operator|++
expr_stmt|;
name|cwd
index|[
name|len
operator|++
index|]
operator|=
literal|'/'
expr_stmt|;
name|cwd
index|[
name|len
index|]
operator|=
literal|0
expr_stmt|;
return|return
name|cwd
operator|+
name|offset
return|;
block|}
end_function
begin_function
DECL|function|git_config_perm
name|int
name|git_config_perm
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
if|if
condition|(
name|value
condition|)
block|{
name|int
name|i
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|value
argument_list|,
literal|"umask"
argument_list|)
condition|)
return|return
name|PERM_UMASK
return|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|value
argument_list|,
literal|"group"
argument_list|)
condition|)
return|return
name|PERM_GROUP
return|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|value
argument_list|,
literal|"all"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|value
argument_list|,
literal|"world"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|value
argument_list|,
literal|"everybody"
argument_list|)
condition|)
return|return
name|PERM_EVERYBODY
return|;
name|i
operator|=
name|atoi
argument_list|(
name|value
argument_list|)
expr_stmt|;
if|if
condition|(
name|i
operator|>
literal|1
condition|)
return|return
name|i
return|;
block|}
return|return
name|git_config_bool
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|check_repository_format_version
name|int
name|check_repository_format_version
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
if|if
condition|(
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"core.repositoryformatversion"
argument_list|)
operator|==
literal|0
condition|)
name|repository_format_version
operator|=
name|git_config_int
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"core.sharedrepository"
argument_list|)
operator|==
literal|0
condition|)
name|shared_repository
operator|=
name|git_config_perm
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"core.bare"
argument_list|)
operator|==
literal|0
condition|)
block|{
name|is_bare_repository_cfg
operator|=
name|git_config_bool
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
if|if
condition|(
name|is_bare_repository_cfg
operator|==
literal|1
condition|)
name|inside_work_tree
operator|=
operator|-
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"core.worktree"
argument_list|)
operator|==
literal|0
condition|)
block|{
if|if
condition|(
name|git_work_tree_cfg
condition|)
name|free
argument_list|(
name|git_work_tree_cfg
argument_list|)
expr_stmt|;
name|git_work_tree_cfg
operator|=
name|xstrdup
argument_list|(
name|value
argument_list|)
expr_stmt|;
name|inside_work_tree
operator|=
operator|-
literal|1
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|check_repository_format
name|int
name|check_repository_format
parameter_list|(
name|void
parameter_list|)
block|{
return|return
name|check_repository_format_gently
argument_list|(
name|NULL
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|setup_git_directory
specifier|const
name|char
modifier|*
name|setup_git_directory
parameter_list|(
name|void
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|retval
init|=
name|setup_git_directory_gently
argument_list|(
name|NULL
argument_list|)
decl_stmt|;
comment|/* If the work tree is not the default one, recompute prefix */
if|if
condition|(
name|inside_work_tree
operator|<
literal|0
condition|)
block|{
specifier|static
name|char
name|buffer
index|[
name|PATH_MAX
operator|+
literal|1
index|]
decl_stmt|;
name|char
modifier|*
name|rel
decl_stmt|;
if|if
condition|(
name|retval
operator|&&
name|chdir
argument_list|(
name|retval
argument_list|)
condition|)
name|die
argument_list|(
literal|"Could not jump back into original cwd"
argument_list|)
expr_stmt|;
name|rel
operator|=
name|get_relative_cwd
argument_list|(
name|buffer
argument_list|,
name|PATH_MAX
argument_list|,
name|get_git_work_tree
argument_list|()
argument_list|)
expr_stmt|;
return|return
name|rel
operator|&&
operator|*
name|rel
condition|?
name|strcat
argument_list|(
name|rel
argument_list|,
literal|"/"
argument_list|)
else|:
name|NULL
return|;
block|}
return|return
name|retval
return|;
block|}
end_function
end_unit
