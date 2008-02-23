begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_decl_stmt
DECL|variable|boolean
specifier|static
name|int
name|boolean
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|integer
specifier|static
name|int
name|integer
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|string
specifier|static
name|char
modifier|*
name|string
init|=
name|NULL
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
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|usage
index|[]
init|=
block|{
literal|"test-parse-options<options>"
block|,
name|NULL
block|}
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT_BOOLEAN
argument_list|(
literal|'b'
argument_list|,
literal|"boolean"
argument_list|,
operator|&
name|boolean
argument_list|,
literal|"get a boolean"
argument_list|)
block|,
name|OPT_INTEGER
argument_list|(
literal|'i'
argument_list|,
literal|"integer"
argument_list|,
operator|&
name|integer
argument_list|,
literal|"get a integer"
argument_list|)
block|,
name|OPT_INTEGER
argument_list|(
literal|'j'
argument_list|,
name|NULL
argument_list|,
operator|&
name|integer
argument_list|,
literal|"get a integer, too"
argument_list|)
block|,
name|OPT_GROUP
argument_list|(
literal|"string options"
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|'s'
argument_list|,
literal|"string"
argument_list|,
operator|&
name|string
argument_list|,
literal|"string"
argument_list|,
literal|"get a string"
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"string2"
argument_list|,
operator|&
name|string
argument_list|,
literal|"str"
argument_list|,
literal|"get another string"
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"st"
argument_list|,
operator|&
name|string
argument_list|,
literal|"st"
argument_list|,
literal|"get another string (pervert ordering)"
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|'o'
argument_list|,
name|NULL
argument_list|,
operator|&
name|string
argument_list|,
literal|"str"
argument_list|,
literal|"get another string"
argument_list|)
block|,
name|OPT_END
argument_list|()
block|, 	}
decl_stmt|;
name|int
name|i
decl_stmt|;
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
name|usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"boolean: %d\n"
argument_list|,
name|boolean
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"integer: %d\n"
argument_list|,
name|integer
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"string: %s\n"
argument_list|,
name|string
condition|?
name|string
else|:
literal|"(not set)"
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
name|argc
condition|;
name|i
operator|++
control|)
name|printf
argument_list|(
literal|"arg %02d: %s\n"
argument_list|,
name|i
argument_list|,
name|argv
index|[
name|i
index|]
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
