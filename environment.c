begin_unit
begin_comment
comment|/*  * We put all the git config variables in this same object  * file, so that programs can link against the config parser  * without having to link against all the rest of git.  *  * In particular, no need to bring in libz etc unless needed,  * even if you might want to know where the git directory etc  * are.  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_decl_stmt
DECL|variable|git_default_email
name|char
name|git_default_email
index|[
name|MAX_GITNAME
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|git_default_name
name|char
name|git_default_name
index|[
name|MAX_GITNAME
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|use_legacy_headers
name|int
name|use_legacy_headers
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|trust_executable_bit
name|int
name|trust_executable_bit
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|assume_unchanged
name|int
name|assume_unchanged
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|prefer_symlink_refs
name|int
name|prefer_symlink_refs
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|log_all_ref_updates
name|int
name|log_all_ref_updates
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|warn_ambiguous_refs
name|int
name|warn_ambiguous_refs
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|repository_format_version
name|int
name|repository_format_version
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|git_commit_encoding
name|char
modifier|*
name|git_commit_encoding
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|git_log_output_encoding
name|char
modifier|*
name|git_log_output_encoding
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|shared_repository
name|int
name|shared_repository
init|=
name|PERM_UMASK
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|apply_default_whitespace
specifier|const
name|char
modifier|*
name|apply_default_whitespace
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|zlib_compression_level
name|int
name|zlib_compression_level
init|=
name|Z_DEFAULT_COMPRESSION
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pager_in_use
name|int
name|pager_in_use
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pager_use_color
name|int
name|pager_use_color
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|git_dir
specifier|static
specifier|const
name|char
modifier|*
name|git_dir
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|git_object_dir
DECL|variable|git_index_file
DECL|variable|git_refs_dir
DECL|variable|git_graft_file
specifier|static
name|char
modifier|*
name|git_object_dir
decl_stmt|,
modifier|*
name|git_index_file
decl_stmt|,
modifier|*
name|git_refs_dir
decl_stmt|,
modifier|*
name|git_graft_file
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|setup_git_env
specifier|static
name|void
name|setup_git_env
parameter_list|(
name|void
parameter_list|)
block|{
name|git_dir
operator|=
name|getenv
argument_list|(
name|GIT_DIR_ENVIRONMENT
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|git_dir
condition|)
name|git_dir
operator|=
name|DEFAULT_GIT_DIR_ENVIRONMENT
expr_stmt|;
name|git_object_dir
operator|=
name|getenv
argument_list|(
name|DB_ENVIRONMENT
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|git_object_dir
condition|)
block|{
name|git_object_dir
operator|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|git_dir
argument_list|)
operator|+
literal|9
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|git_object_dir
argument_list|,
literal|"%s/objects"
argument_list|,
name|git_dir
argument_list|)
expr_stmt|;
block|}
name|git_refs_dir
operator|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|git_dir
argument_list|)
operator|+
literal|6
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|git_refs_dir
argument_list|,
literal|"%s/refs"
argument_list|,
name|git_dir
argument_list|)
expr_stmt|;
name|git_index_file
operator|=
name|getenv
argument_list|(
name|INDEX_ENVIRONMENT
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|git_index_file
condition|)
block|{
name|git_index_file
operator|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|git_dir
argument_list|)
operator|+
literal|7
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|git_index_file
argument_list|,
literal|"%s/index"
argument_list|,
name|git_dir
argument_list|)
expr_stmt|;
block|}
name|git_graft_file
operator|=
name|getenv
argument_list|(
name|GRAFT_ENVIRONMENT
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|git_graft_file
condition|)
name|git_graft_file
operator|=
name|xstrdup
argument_list|(
name|git_path
argument_list|(
literal|"info/grafts"
argument_list|)
argument_list|)
expr_stmt|;
name|log_all_ref_updates
operator|=
operator|!
name|is_bare_git_dir
argument_list|(
name|git_dir
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|is_bare_git_dir
name|int
name|is_bare_git_dir
parameter_list|(
specifier|const
name|char
modifier|*
name|dir
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|s
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|dir
argument_list|,
name|DEFAULT_GIT_DIR_ENVIRONMENT
argument_list|)
condition|)
return|return
literal|0
return|;
name|s
operator|=
name|strrchr
argument_list|(
name|dir
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
return|return
operator|!
name|s
operator|||
name|strcmp
argument_list|(
name|s
operator|+
literal|1
argument_list|,
name|DEFAULT_GIT_DIR_ENVIRONMENT
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|get_git_dir
specifier|const
name|char
modifier|*
name|get_git_dir
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
operator|!
name|git_dir
condition|)
name|setup_git_env
argument_list|()
expr_stmt|;
return|return
name|git_dir
return|;
block|}
end_function
begin_function
DECL|function|get_object_directory
name|char
modifier|*
name|get_object_directory
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
operator|!
name|git_object_dir
condition|)
name|setup_git_env
argument_list|()
expr_stmt|;
return|return
name|git_object_dir
return|;
block|}
end_function
begin_function
DECL|function|get_refs_directory
name|char
modifier|*
name|get_refs_directory
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
operator|!
name|git_refs_dir
condition|)
name|setup_git_env
argument_list|()
expr_stmt|;
return|return
name|git_refs_dir
return|;
block|}
end_function
begin_function
DECL|function|get_index_file
name|char
modifier|*
name|get_index_file
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
operator|!
name|git_index_file
condition|)
name|setup_git_env
argument_list|()
expr_stmt|;
return|return
name|git_index_file
return|;
block|}
end_function
begin_function
DECL|function|get_graft_file
name|char
modifier|*
name|get_graft_file
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
operator|!
name|git_graft_file
condition|)
name|setup_git_env
argument_list|()
expr_stmt|;
return|return
name|git_graft_file
return|;
block|}
end_function
end_unit
