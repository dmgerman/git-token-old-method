begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_decl_stmt
DECL|variable|git_update_ref_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|git_update_ref_usage
index|[]
init|=
block|{
literal|"git-update-ref [options] -d<refname> [<oldval>]"
block|,
literal|"git-update-ref [options]<refname><newval> [<oldval>]"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cmd_update_ref
name|int
name|cmd_update_ref
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
specifier|const
name|char
modifier|*
name|refname
decl_stmt|,
modifier|*
name|oldval
decl_stmt|,
modifier|*
name|msg
init|=
name|NULL
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|,
name|oldsha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|delete
init|=
literal|0
decl_stmt|,
name|no_deref
init|=
literal|0
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT_STRING
argument_list|(
literal|'m'
argument_list|,
name|NULL
argument_list|,
operator|&
name|msg
argument_list|,
literal|"reason"
argument_list|,
literal|"reason of the update"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'d'
argument_list|,
name|NULL
argument_list|,
operator|&
name|delete
argument_list|,
literal|"deletes the reference"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"no-deref"
argument_list|,
operator|&
name|no_deref
argument_list|,
literal|"update<refname> not the one it points to"
argument_list|)
block|,
name|OPT_END
argument_list|()
block|, 	}
decl_stmt|;
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
name|options
argument_list|,
name|git_update_ref_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|msg
operator|&&
operator|!
operator|*
name|msg
condition|)
name|die
argument_list|(
literal|"Refusing to perform update with empty message."
argument_list|)
expr_stmt|;
if|if
condition|(
name|delete
condition|)
block|{
if|if
condition|(
name|argc
operator|<
literal|1
operator|||
name|argc
operator|>
literal|2
condition|)
name|usage_with_options
argument_list|(
name|git_update_ref_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
name|refname
operator|=
name|argv
index|[
literal|0
index|]
expr_stmt|;
name|oldval
operator|=
name|argv
index|[
literal|1
index|]
expr_stmt|;
block|}
else|else
block|{
specifier|const
name|char
modifier|*
name|value
decl_stmt|;
if|if
condition|(
name|argc
operator|<
literal|2
operator|||
name|argc
operator|>
literal|3
condition|)
name|usage_with_options
argument_list|(
name|git_update_ref_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
name|refname
operator|=
name|argv
index|[
literal|0
index|]
expr_stmt|;
name|value
operator|=
name|argv
index|[
literal|1
index|]
expr_stmt|;
name|oldval
operator|=
name|argv
index|[
literal|2
index|]
expr_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|value
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"%s: not a valid SHA1"
argument_list|,
name|value
argument_list|)
expr_stmt|;
block|}
name|hashclr
argument_list|(
name|oldsha1
argument_list|)
expr_stmt|;
comment|/* all-zero hash in case oldval is the empty string */
if|if
condition|(
name|oldval
operator|&&
operator|*
name|oldval
operator|&&
name|get_sha1
argument_list|(
name|oldval
argument_list|,
name|oldsha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"%s: not a valid old SHA1"
argument_list|,
name|oldval
argument_list|)
expr_stmt|;
if|if
condition|(
name|delete
condition|)
return|return
name|delete_ref
argument_list|(
name|refname
argument_list|,
name|oldval
condition|?
name|oldsha1
else|:
name|NULL
argument_list|)
return|;
else|else
return|return
name|update_ref
argument_list|(
name|msg
argument_list|,
name|refname
argument_list|,
name|sha1
argument_list|,
name|oldval
condition|?
name|oldsha1
else|:
name|NULL
argument_list|,
name|no_deref
condition|?
name|REF_NODEREF
else|:
literal|0
argument_list|,
name|DIE_ON_ERR
argument_list|)
return|;
block|}
end_function
end_unit
