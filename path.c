begin_unit
begin_comment
comment|/*  * I'm tired of doing "vsnprintf()" etc just to open a  * file, so here's a "return static buffer with printf"  * interface for paths.  *  * It's obviously not thread-safe. Sue me. But it's quite  * useful for doing things like  *  *   f = open(mkpath("%s/%s.git", base, name), O_RDONLY);  *  * which is what it's designed for.  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_decl_stmt
DECL|variable|pathname
specifier|static
name|char
name|pathname
index|[
name|PATH_MAX
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|bad_path
specifier|static
name|char
name|bad_path
index|[]
init|=
literal|"/bad-path/"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cleanup_path
specifier|static
name|char
modifier|*
name|cleanup_path
parameter_list|(
name|char
modifier|*
name|path
parameter_list|)
block|{
comment|/* Clean it up */
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|path
argument_list|,
literal|"./"
argument_list|,
literal|2
argument_list|)
condition|)
block|{
name|path
operator|+=
literal|2
expr_stmt|;
while|while
condition|(
operator|*
name|path
operator|==
literal|'/'
condition|)
name|path
operator|++
expr_stmt|;
block|}
return|return
name|path
return|;
block|}
end_function
begin_function
DECL|function|mkpath
name|char
modifier|*
name|mkpath
parameter_list|(
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
block|{
name|va_list
name|args
decl_stmt|;
name|unsigned
name|len
decl_stmt|;
name|va_start
argument_list|(
name|args
argument_list|,
name|fmt
argument_list|)
expr_stmt|;
name|len
operator|=
name|vsnprintf
argument_list|(
name|pathname
argument_list|,
name|PATH_MAX
argument_list|,
name|fmt
argument_list|,
name|args
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|args
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|>=
name|PATH_MAX
condition|)
return|return
name|bad_path
return|;
return|return
name|cleanup_path
argument_list|(
name|pathname
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|git_path
name|char
modifier|*
name|git_path
parameter_list|(
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|git_dir
init|=
name|gitenv
argument_list|(
name|GIT_DIR_ENVIRONMENT
argument_list|)
condition|?
else|:
name|DEFAULT_GIT_DIR_ENVIRONMENT
decl_stmt|;
name|va_list
name|args
decl_stmt|;
name|unsigned
name|len
decl_stmt|;
name|len
operator|=
name|strlen
argument_list|(
name|git_dir
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|>
name|PATH_MAX
operator|-
literal|100
condition|)
return|return
name|bad_path
return|;
name|memcpy
argument_list|(
name|pathname
argument_list|,
name|git_dir
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|&&
name|git_dir
index|[
name|len
operator|-
literal|1
index|]
operator|!=
literal|'/'
condition|)
name|pathname
index|[
name|len
operator|++
index|]
operator|=
literal|'/'
expr_stmt|;
name|va_start
argument_list|(
name|args
argument_list|,
name|fmt
argument_list|)
expr_stmt|;
name|len
operator|+=
name|vsnprintf
argument_list|(
name|pathname
operator|+
name|len
argument_list|,
name|PATH_MAX
operator|-
name|len
argument_list|,
name|fmt
argument_list|,
name|args
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|args
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|>=
name|PATH_MAX
condition|)
return|return
name|bad_path
return|;
return|return
name|cleanup_path
argument_list|(
name|pathname
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/* git_mkstemp() - create tmp file honoring TMPDIR variable */
end_comment
begin_function
DECL|function|git_mkstemp
name|int
name|git_mkstemp
parameter_list|(
name|char
modifier|*
name|path
parameter_list|,
name|size_t
name|len
parameter_list|,
specifier|const
name|char
modifier|*
name|template
parameter_list|)
block|{
name|char
modifier|*
name|env
decl_stmt|,
modifier|*
name|pch
init|=
name|path
decl_stmt|;
if|if
condition|(
operator|(
name|env
operator|=
name|getenv
argument_list|(
literal|"TMPDIR"
argument_list|)
operator|)
operator|==
name|NULL
condition|)
block|{
name|strcpy
argument_list|(
name|pch
argument_list|,
literal|"/tmp/"
argument_list|)
expr_stmt|;
name|len
operator|-=
literal|5
expr_stmt|;
block|}
else|else
name|len
operator|-=
name|snprintf
argument_list|(
name|pch
argument_list|,
name|len
argument_list|,
literal|"%s/"
argument_list|,
name|env
argument_list|)
expr_stmt|;
name|safe_strncpy
argument_list|(
name|pch
argument_list|,
name|template
argument_list|,
name|len
argument_list|)
expr_stmt|;
return|return
name|mkstemp
argument_list|(
name|path
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|safe_strncpy
name|char
modifier|*
name|safe_strncpy
parameter_list|(
name|char
modifier|*
name|dest
parameter_list|,
specifier|const
name|char
modifier|*
name|src
parameter_list|,
name|size_t
name|n
parameter_list|)
block|{
name|strncpy
argument_list|(
name|dest
argument_list|,
name|src
argument_list|,
name|n
argument_list|)
expr_stmt|;
name|dest
index|[
name|n
operator|-
literal|1
index|]
operator|=
literal|'\0'
expr_stmt|;
return|return
name|dest
return|;
block|}
end_function
end_unit
