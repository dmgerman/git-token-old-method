begin_unit
begin_comment
comment|/*  * test-obj-pool.c: code to exercise the svn importer's object pool  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"vcs-svn/obj_pool.h"
end_include
begin_enum
DECL|enum|pool
DECL|enumerator|POOL_ONE
DECL|enumerator|POOL_TWO
enum|enum
name|pool
block|{
name|POOL_ONE
block|,
name|POOL_TWO
block|}
enum|;
end_enum
begin_macro
name|obj_pool_gen
argument_list|(
argument|one
argument_list|,
argument|int
argument_list|,
literal|1
argument_list|)
end_macro
begin_macro
name|obj_pool_gen
argument_list|(
argument|two
argument_list|,
argument|int
argument_list|,
literal|4096
argument_list|)
end_macro
begin_function
DECL|function|strtouint32
specifier|static
name|uint32_t
name|strtouint32
parameter_list|(
specifier|const
name|char
modifier|*
name|s
parameter_list|)
block|{
name|char
modifier|*
name|end
decl_stmt|;
name|uintmax_t
name|n
init|=
name|strtoumax
argument_list|(
name|s
argument_list|,
operator|&
name|end
argument_list|,
literal|10
argument_list|)
decl_stmt|;
if|if
condition|(
operator|*
name|s
operator|==
literal|'\0'
operator|||
operator|(
operator|*
name|end
operator|!=
literal|'\n'
operator|&&
operator|*
name|end
operator|!=
literal|'\0'
operator|)
condition|)
name|die
argument_list|(
literal|"invalid offset: %s"
argument_list|,
name|s
argument_list|)
expr_stmt|;
return|return
operator|(
name|uint32_t
operator|)
name|n
return|;
block|}
end_function
begin_function
DECL|function|handle_command
specifier|static
name|void
name|handle_command
parameter_list|(
specifier|const
name|char
modifier|*
name|command
parameter_list|,
name|enum
name|pool
name|pool
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|)
block|{
switch|switch
condition|(
operator|*
name|command
condition|)
block|{
case|case
literal|'a'
case|:
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|command
argument_list|,
literal|"alloc "
argument_list|)
condition|)
block|{
name|uint32_t
name|n
init|=
name|strtouint32
argument_list|(
name|arg
argument_list|)
decl_stmt|;
name|printf
argument_list|(
literal|"%"
name|PRIu32
literal|"\n"
argument_list|,
name|pool
operator|==
name|POOL_ONE
condition|?
name|one_alloc
argument_list|(
name|n
argument_list|)
else|:
name|two_alloc
argument_list|(
name|n
argument_list|)
argument_list|)
expr_stmt|;
return|return;
block|}
case|case
literal|'c'
case|:
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|command
argument_list|,
literal|"commit "
argument_list|)
condition|)
block|{
name|pool
operator|==
name|POOL_ONE
condition|?
name|one_commit
argument_list|()
else|:
name|two_commit
argument_list|()
expr_stmt|;
return|return;
block|}
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|command
argument_list|,
literal|"committed "
argument_list|)
condition|)
block|{
name|printf
argument_list|(
literal|"%"
name|PRIu32
literal|"\n"
argument_list|,
name|pool
operator|==
name|POOL_ONE
condition|?
name|one_pool
operator|.
name|committed
else|:
name|two_pool
operator|.
name|committed
argument_list|)
expr_stmt|;
return|return;
block|}
case|case
literal|'f'
case|:
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|command
argument_list|,
literal|"free "
argument_list|)
condition|)
block|{
name|uint32_t
name|n
init|=
name|strtouint32
argument_list|(
name|arg
argument_list|)
decl_stmt|;
name|pool
operator|==
name|POOL_ONE
condition|?
name|one_free
argument_list|(
name|n
argument_list|)
else|:
name|two_free
argument_list|(
name|n
argument_list|)
expr_stmt|;
return|return;
block|}
case|case
literal|'n'
case|:
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|command
argument_list|,
literal|"null "
argument_list|)
condition|)
block|{
name|printf
argument_list|(
literal|"%"
name|PRIu32
literal|"\n"
argument_list|,
name|pool
operator|==
name|POOL_ONE
condition|?
name|one_offset
argument_list|(
name|NULL
argument_list|)
else|:
name|two_offset
argument_list|(
name|NULL
argument_list|)
argument_list|)
expr_stmt|;
return|return;
block|}
case|case
literal|'o'
case|:
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|command
argument_list|,
literal|"offset "
argument_list|)
condition|)
block|{
name|uint32_t
name|n
init|=
name|strtouint32
argument_list|(
name|arg
argument_list|)
decl_stmt|;
name|printf
argument_list|(
literal|"%"
name|PRIu32
literal|"\n"
argument_list|,
name|pool
operator|==
name|POOL_ONE
condition|?
name|one_offset
argument_list|(
name|one_pointer
argument_list|(
name|n
argument_list|)
argument_list|)
else|:
name|two_offset
argument_list|(
name|two_pointer
argument_list|(
name|n
argument_list|)
argument_list|)
argument_list|)
expr_stmt|;
return|return;
block|}
case|case
literal|'r'
case|:
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|command
argument_list|,
literal|"reset "
argument_list|)
condition|)
block|{
name|pool
operator|==
name|POOL_ONE
condition|?
name|one_reset
argument_list|()
else|:
name|two_reset
argument_list|()
expr_stmt|;
return|return;
block|}
case|case
literal|'s'
case|:
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|command
argument_list|,
literal|"set "
argument_list|)
condition|)
block|{
name|uint32_t
name|n
init|=
name|strtouint32
argument_list|(
name|arg
argument_list|)
decl_stmt|;
if|if
condition|(
name|pool
operator|==
name|POOL_ONE
condition|)
operator|*
name|one_pointer
argument_list|(
name|n
argument_list|)
operator|=
literal|1
expr_stmt|;
else|else
operator|*
name|two_pointer
argument_list|(
name|n
argument_list|)
operator|=
literal|1
expr_stmt|;
return|return;
block|}
case|case
literal|'t'
case|:
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|command
argument_list|,
literal|"test "
argument_list|)
condition|)
block|{
name|uint32_t
name|n
init|=
name|strtouint32
argument_list|(
name|arg
argument_list|)
decl_stmt|;
name|printf
argument_list|(
literal|"%d\n"
argument_list|,
name|pool
operator|==
name|POOL_ONE
condition|?
operator|*
name|one_pointer
argument_list|(
name|n
argument_list|)
else|:
operator|*
name|two_pointer
argument_list|(
name|n
argument_list|)
argument_list|)
expr_stmt|;
return|return;
block|}
default|default:
name|die
argument_list|(
literal|"unrecognized command: %s"
argument_list|,
name|command
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|handle_line
specifier|static
name|void
name|handle_line
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|arg
init|=
name|strchr
argument_list|(
name|line
argument_list|,
literal|' '
argument_list|)
decl_stmt|;
name|enum
name|pool
name|pool
decl_stmt|;
if|if
condition|(
name|arg
operator|&&
operator|!
name|prefixcmp
argument_list|(
name|arg
operator|+
literal|1
argument_list|,
literal|"one"
argument_list|)
condition|)
name|pool
operator|=
name|POOL_ONE
expr_stmt|;
elseif|else
if|if
condition|(
name|arg
operator|&&
operator|!
name|prefixcmp
argument_list|(
name|arg
operator|+
literal|1
argument_list|,
literal|"two"
argument_list|)
condition|)
name|pool
operator|=
name|POOL_TWO
expr_stmt|;
else|else
name|die
argument_list|(
literal|"no pool specified: %s"
argument_list|,
name|line
argument_list|)
expr_stmt|;
name|handle_command
argument_list|(
name|line
argument_list|,
name|pool
argument_list|,
name|arg
operator|+
name|strlen
argument_list|(
literal|"one "
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
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
name|argv
index|[]
parameter_list|)
block|{
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
if|if
condition|(
name|argc
operator|!=
literal|1
condition|)
name|usage
argument_list|(
literal|"test-obj-str< script"
argument_list|)
expr_stmt|;
while|while
condition|(
name|strbuf_getline
argument_list|(
operator|&
name|sb
argument_list|,
name|stdin
argument_list|,
literal|'\n'
argument_list|)
operator|!=
name|EOF
condition|)
name|handle_line
argument_list|(
name|sb
operator|.
name|buf
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
