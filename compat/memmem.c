begin_unit
begin_include
include|#
directive|include
file|"../git-compat-util.h"
end_include
begin_function
DECL|function|gitmemmem
name|void
modifier|*
name|gitmemmem
parameter_list|(
specifier|const
name|void
modifier|*
name|haystack
parameter_list|,
name|size_t
name|haystack_len
parameter_list|,
specifier|const
name|void
modifier|*
name|needle
parameter_list|,
name|size_t
name|needle_len
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|begin
init|=
name|haystack
decl_stmt|;
specifier|const
name|char
modifier|*
name|last_possible
init|=
name|begin
operator|+
name|haystack_len
operator|-
name|needle_len
decl_stmt|;
specifier|const
name|char
modifier|*
name|tail
init|=
name|needle
decl_stmt|;
name|char
name|point
decl_stmt|;
comment|/* 	 * The first occurrence of the empty string is deemed to occur at 	 * the beginning of the string. 	 */
if|if
condition|(
name|needle_len
operator|==
literal|0
condition|)
return|return
operator|(
name|void
operator|*
operator|)
name|begin
return|;
comment|/* 	 * Sanity check, otherwise the loop might search through the whole 	 * memory. 	 */
if|if
condition|(
name|haystack_len
operator|<
name|needle_len
condition|)
return|return
name|NULL
return|;
name|point
operator|=
operator|*
name|tail
operator|++
expr_stmt|;
for|for
control|(
init|;
name|begin
operator|<=
name|last_possible
condition|;
name|begin
operator|++
control|)
block|{
if|if
condition|(
operator|*
name|begin
operator|==
name|point
operator|&&
operator|!
name|memcmp
argument_list|(
name|begin
operator|+
literal|1
argument_list|,
name|tail
argument_list|,
name|needle_len
operator|-
literal|1
argument_list|)
condition|)
return|return
operator|(
name|void
operator|*
operator|)
name|begin
return|;
block|}
return|return
name|NULL
return|;
block|}
end_function
end_unit
