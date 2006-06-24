begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|ac
parameter_list|,
name|char
modifier|*
modifier|*
name|av
parameter_list|)
block|{
name|SHA_CTX
name|ctx
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|SHA1_Init
argument_list|(
operator|&
name|ctx
argument_list|)
expr_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
name|ssize_t
name|sz
decl_stmt|;
name|char
name|buffer
index|[
literal|8192
index|]
decl_stmt|;
name|sz
operator|=
name|xread
argument_list|(
literal|0
argument_list|,
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|sz
operator|==
literal|0
condition|)
break|break;
if|if
condition|(
name|sz
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"test-sha1: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|SHA1_Update
argument_list|(
operator|&
name|ctx
argument_list|,
name|buffer
argument_list|,
name|sz
argument_list|)
expr_stmt|;
block|}
name|SHA1_Final
argument_list|(
name|sha1
argument_list|,
operator|&
name|ctx
argument_list|)
expr_stmt|;
name|puts
argument_list|(
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|0
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
