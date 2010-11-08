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
DECL|variable|timestamp
specifier|static
name|unsigned
name|long
name|timestamp
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|abbrev
specifier|static
name|int
name|abbrev
init|=
literal|7
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|verbose
DECL|variable|dry_run
DECL|variable|quiet
specifier|static
name|int
name|verbose
init|=
literal|0
decl_stmt|,
name|dry_run
init|=
literal|0
decl_stmt|,
name|quiet
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
begin_decl_stmt
DECL|variable|file
specifier|static
name|char
modifier|*
name|file
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|ambiguous
specifier|static
name|int
name|ambiguous
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|length_callback
specifier|static
name|int
name|length_callback
parameter_list|(
specifier|const
name|struct
name|option
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
block|{
name|printf
argument_list|(
literal|"Callback: \"%s\", %d\n"
argument_list|,
operator|(
name|arg
condition|?
name|arg
else|:
literal|"not set"
operator|)
argument_list|,
name|unset
argument_list|)
expr_stmt|;
if|if
condition|(
name|unset
condition|)
return|return
literal|1
return|;
comment|/* do not support unset */
operator|*
operator|(
name|int
operator|*
operator|)
name|opt
operator|->
name|value
operator|=
name|strlen
argument_list|(
name|arg
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|number_callback
specifier|static
name|int
name|number_callback
parameter_list|(
specifier|const
name|struct
name|option
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
block|{
operator|*
operator|(
name|int
operator|*
operator|)
name|opt
operator|->
name|value
operator|=
name|strtol
argument_list|(
name|arg
argument_list|,
name|NULL
argument_list|,
literal|10
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
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
name|prefix
init|=
literal|"prefix/"
decl_stmt|;
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
name|OPT_BIT
argument_list|(
literal|'4'
argument_list|,
literal|"or4"
argument_list|,
operator|&
name|boolean
argument_list|,
literal|"bitwise-or boolean with ...0100"
argument_list|,
literal|4
argument_list|)
block|,
name|OPT_NEGBIT
argument_list|(
literal|0
argument_list|,
literal|"neg-or4"
argument_list|,
operator|&
name|boolean
argument_list|,
literal|"same as --no-or4"
argument_list|,
literal|4
argument_list|)
block|,
name|OPT_GROUP
argument_list|(
literal|""
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
name|OPT_SET_INT
argument_list|(
literal|0
argument_list|,
literal|"set23"
argument_list|,
operator|&
name|integer
argument_list|,
literal|"set integer to 23"
argument_list|,
literal|23
argument_list|)
block|,
name|OPT_DATE
argument_list|(
literal|'t'
argument_list|,
name|NULL
argument_list|,
operator|&
name|timestamp
argument_list|,
literal|"get timestamp of<time>"
argument_list|)
block|,
name|OPT_CALLBACK
argument_list|(
literal|'L'
argument_list|,
literal|"length"
argument_list|,
operator|&
name|integer
argument_list|,
literal|"str"
argument_list|,
literal|"get length of<str>"
argument_list|,
name|length_callback
argument_list|)
block|,
name|OPT_FILENAME
argument_list|(
literal|'F'
argument_list|,
literal|"file"
argument_list|,
operator|&
name|file
argument_list|,
literal|"set file to<FILE>"
argument_list|)
block|,
name|OPT_GROUP
argument_list|(
literal|"String options"
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
name|OPT_SET_PTR
argument_list|(
literal|0
argument_list|,
literal|"default-string"
argument_list|,
operator|&
name|string
argument_list|,
literal|"set string to default"
argument_list|,
operator|(
name|unsigned
name|long
operator|)
literal|"default"
argument_list|)
block|,
name|OPT_GROUP
argument_list|(
literal|"Magic arguments"
argument_list|)
block|,
name|OPT_ARGUMENT
argument_list|(
literal|"quux"
argument_list|,
literal|"means --quux"
argument_list|)
block|,
name|OPT_NUMBER_CALLBACK
argument_list|(
operator|&
name|integer
argument_list|,
literal|"set integer to NUM"
argument_list|,
name|number_callback
argument_list|)
block|,
block|{
name|OPTION_BOOLEAN
block|,
literal|'+'
block|,
name|NULL
block|,
operator|&
name|boolean
block|,
name|NULL
block|,
literal|"same as -b"
block|,
name|PARSE_OPT_NOARG
operator||
name|PARSE_OPT_NONEG
operator||
name|PARSE_OPT_NODASH
block|}
block|,
block|{
name|OPTION_BOOLEAN
block|,
literal|0
block|,
literal|"ambiguous"
block|,
operator|&
name|ambiguous
block|,
name|NULL
block|,
literal|"positive ambiguity"
block|,
name|PARSE_OPT_NOARG
operator||
name|PARSE_OPT_NONEG
block|}
block|,
block|{
name|OPTION_BOOLEAN
block|,
literal|0
block|,
literal|"no-ambiguous"
block|,
operator|&
name|ambiguous
block|,
name|NULL
block|,
literal|"negative ambiguity"
block|,
name|PARSE_OPT_NOARG
operator||
name|PARSE_OPT_NONEG
block|}
block|,
name|OPT_GROUP
argument_list|(
literal|"Standard options"
argument_list|)
block|,
name|OPT__ABBREV
argument_list|(
operator|&
name|abbrev
argument_list|)
block|,
name|OPT__VERBOSE
argument_list|(
operator|&
name|verbose
argument_list|,
literal|"be verbose"
argument_list|)
block|,
name|OPT__DRY_RUN
argument_list|(
operator|&
name|dry_run
argument_list|,
literal|"dry run"
argument_list|)
block|,
name|OPT__QUIET
argument_list|(
operator|&
name|quiet
argument_list|,
literal|"be quiet"
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
name|prefix
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
literal|"integer: %u\n"
argument_list|,
name|integer
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"timestamp: %lu\n"
argument_list|,
name|timestamp
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
name|printf
argument_list|(
literal|"abbrev: %d\n"
argument_list|,
name|abbrev
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"verbose: %d\n"
argument_list|,
name|verbose
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"quiet: %s\n"
argument_list|,
name|quiet
condition|?
literal|"yes"
else|:
literal|"no"
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"dry run: %s\n"
argument_list|,
name|dry_run
condition|?
literal|"yes"
else|:
literal|"no"
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"file: %s\n"
argument_list|,
name|file
condition|?
name|file
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
