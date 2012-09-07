begin_unit
begin_comment
comment|/*  * Builtin "git replace"  *  * Copyright (c) 2008 Christian Couder<chriscool@tuxfamily.org>  *  * Based on builtin-tag.c by Kristian HÃ¸gsberg<krh@redhat.com>  * and Carlos Rica<jasampler@gmail.com> that was itself based on  * git-tag.sh and mktag.c by Linus Torvalds.  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_decl_stmt
DECL|variable|git_replace_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|git_replace_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git replace [-f]<object><replacement>"
argument_list|)
block|,
name|N_
argument_list|(
literal|"git replace -d<object>..."
argument_list|)
block|,
name|N_
argument_list|(
literal|"git replace -l [<pattern>]"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|show_reference
specifier|static
name|int
name|show_reference
parameter_list|(
specifier|const
name|char
modifier|*
name|refname
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|flag
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|pattern
init|=
name|cb_data
decl_stmt|;
if|if
condition|(
operator|!
name|fnmatch
argument_list|(
name|pattern
argument_list|,
name|refname
argument_list|,
literal|0
argument_list|)
condition|)
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|refname
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|list_replace_refs
specifier|static
name|int
name|list_replace_refs
parameter_list|(
specifier|const
name|char
modifier|*
name|pattern
parameter_list|)
block|{
if|if
condition|(
name|pattern
operator|==
name|NULL
condition|)
name|pattern
operator|=
literal|"*"
expr_stmt|;
name|for_each_replace_ref
argument_list|(
name|show_reference
argument_list|,
operator|(
name|void
operator|*
operator|)
name|pattern
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_typedef
DECL|typedef|each_replace_name_fn
typedef|typedef
name|int
function_decl|(
modifier|*
name|each_replace_name_fn
function_decl|)
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|char
modifier|*
name|ref
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_typedef
begin_function
DECL|function|for_each_replace_name
specifier|static
name|int
name|for_each_replace_name
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|each_replace_name_fn
name|fn
parameter_list|)
block|{
specifier|const
name|char
modifier|*
modifier|*
name|p
decl_stmt|;
name|char
name|ref
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|int
name|had_error
init|=
literal|0
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
for|for
control|(
name|p
operator|=
name|argv
init|;
operator|*
name|p
condition|;
name|p
operator|++
control|)
block|{
if|if
condition|(
name|snprintf
argument_list|(
name|ref
argument_list|,
sizeof|sizeof
argument_list|(
name|ref
argument_list|)
argument_list|,
literal|"refs/replace/%s"
argument_list|,
operator|*
name|p
argument_list|)
operator|>=
sizeof|sizeof
argument_list|(
name|ref
argument_list|)
condition|)
block|{
name|error
argument_list|(
literal|"replace ref name too long: %.*s..."
argument_list|,
literal|50
argument_list|,
operator|*
name|p
argument_list|)
expr_stmt|;
name|had_error
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|read_ref
argument_list|(
name|ref
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
name|error
argument_list|(
literal|"replace ref '%s' not found."
argument_list|,
operator|*
name|p
argument_list|)
expr_stmt|;
name|had_error
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|fn
argument_list|(
operator|*
name|p
argument_list|,
name|ref
argument_list|,
name|sha1
argument_list|)
condition|)
name|had_error
operator|=
literal|1
expr_stmt|;
block|}
return|return
name|had_error
return|;
block|}
end_function
begin_function
DECL|function|delete_replace_ref
specifier|static
name|int
name|delete_replace_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|char
modifier|*
name|ref
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
if|if
condition|(
name|delete_ref
argument_list|(
name|ref
argument_list|,
name|sha1
argument_list|,
literal|0
argument_list|)
condition|)
return|return
literal|1
return|;
name|printf
argument_list|(
literal|"Deleted replace ref '%s'\n"
argument_list|,
name|name
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|replace_object
specifier|static
name|int
name|replace_object
parameter_list|(
specifier|const
name|char
modifier|*
name|object_ref
parameter_list|,
specifier|const
name|char
modifier|*
name|replace_ref
parameter_list|,
name|int
name|force
parameter_list|)
block|{
name|unsigned
name|char
name|object
index|[
literal|20
index|]
decl_stmt|,
name|prev
index|[
literal|20
index|]
decl_stmt|,
name|repl
index|[
literal|20
index|]
decl_stmt|;
name|char
name|ref
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|struct
name|ref_lock
modifier|*
name|lock
decl_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|object_ref
argument_list|,
name|object
argument_list|)
condition|)
name|die
argument_list|(
literal|"Failed to resolve '%s' as a valid ref."
argument_list|,
name|object_ref
argument_list|)
expr_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|replace_ref
argument_list|,
name|repl
argument_list|)
condition|)
name|die
argument_list|(
literal|"Failed to resolve '%s' as a valid ref."
argument_list|,
name|replace_ref
argument_list|)
expr_stmt|;
if|if
condition|(
name|snprintf
argument_list|(
name|ref
argument_list|,
sizeof|sizeof
argument_list|(
name|ref
argument_list|)
argument_list|,
literal|"refs/replace/%s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|object
argument_list|)
argument_list|)
operator|>
sizeof|sizeof
argument_list|(
name|ref
argument_list|)
operator|-
literal|1
condition|)
name|die
argument_list|(
literal|"replace ref name too long: %.*s..."
argument_list|,
literal|50
argument_list|,
name|ref
argument_list|)
expr_stmt|;
if|if
condition|(
name|check_refname_format
argument_list|(
name|ref
argument_list|,
literal|0
argument_list|)
condition|)
name|die
argument_list|(
literal|"'%s' is not a valid ref name."
argument_list|,
name|ref
argument_list|)
expr_stmt|;
if|if
condition|(
name|read_ref
argument_list|(
name|ref
argument_list|,
name|prev
argument_list|)
condition|)
name|hashclr
argument_list|(
name|prev
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|force
condition|)
name|die
argument_list|(
literal|"replace ref '%s' already exists"
argument_list|,
name|ref
argument_list|)
expr_stmt|;
name|lock
operator|=
name|lock_any_ref_for_update
argument_list|(
name|ref
argument_list|,
name|prev
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|lock
condition|)
name|die
argument_list|(
literal|"%s: cannot lock the ref"
argument_list|,
name|ref
argument_list|)
expr_stmt|;
if|if
condition|(
name|write_ref_sha1
argument_list|(
name|lock
argument_list|,
name|repl
argument_list|,
name|NULL
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"%s: cannot update the ref"
argument_list|,
name|ref
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|cmd_replace
name|int
name|cmd_replace
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
name|list
init|=
literal|0
decl_stmt|,
name|delete
init|=
literal|0
decl_stmt|,
name|force
init|=
literal|0
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT_BOOLEAN
argument_list|(
literal|'l'
argument_list|,
name|NULL
argument_list|,
operator|&
name|list
argument_list|,
name|N_
argument_list|(
literal|"list replace refs"
argument_list|)
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
name|N_
argument_list|(
literal|"delete replace refs"
argument_list|)
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'f'
argument_list|,
name|NULL
argument_list|,
operator|&
name|force
argument_list|,
name|N_
argument_list|(
literal|"replace the ref if it exists"
argument_list|)
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
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
name|options
argument_list|,
name|git_replace_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|list
operator|&&
name|delete
condition|)
name|usage_msg_opt
argument_list|(
literal|"-l and -d cannot be used together"
argument_list|,
name|git_replace_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
if|if
condition|(
name|force
operator|&&
operator|(
name|list
operator|||
name|delete
operator|)
condition|)
name|usage_msg_opt
argument_list|(
literal|"-f cannot be used with -d or -l"
argument_list|,
name|git_replace_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
comment|/* Delete refs */
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
condition|)
name|usage_msg_opt
argument_list|(
literal|"-d needs at least one argument"
argument_list|,
name|git_replace_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
return|return
name|for_each_replace_name
argument_list|(
name|argv
argument_list|,
name|delete_replace_ref
argument_list|)
return|;
block|}
comment|/* Replace object */
if|if
condition|(
operator|!
name|list
operator|&&
name|argc
condition|)
block|{
if|if
condition|(
name|argc
operator|!=
literal|2
condition|)
name|usage_msg_opt
argument_list|(
literal|"bad number of arguments"
argument_list|,
name|git_replace_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
return|return
name|replace_object
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|,
name|force
argument_list|)
return|;
block|}
comment|/* List refs, even if "list" is not set */
if|if
condition|(
name|argc
operator|>
literal|1
condition|)
name|usage_msg_opt
argument_list|(
literal|"only one pattern can be given with -l"
argument_list|,
name|git_replace_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
if|if
condition|(
name|force
condition|)
name|usage_msg_opt
argument_list|(
literal|"-f needs some arguments"
argument_list|,
name|git_replace_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
return|return
name|list_replace_refs
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|)
return|;
block|}
end_function
end_unit
