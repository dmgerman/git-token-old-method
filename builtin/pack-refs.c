begin_unit
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
begin_include
include|#
directive|include
file|"pack-refs.h"
end_include
begin_decl_stmt
DECL|variable|pack_refs_usage
specifier|static
name|char
specifier|const
modifier|*
specifier|const
name|pack_refs_usage
index|[]
init|=
block|{
literal|"git pack-refs [options]"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cmd_pack_refs
name|int
name|cmd_pack_refs
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
name|unsigned
name|int
name|flags
init|=
name|PACK_REFS_PRUNE
decl_stmt|;
name|struct
name|option
name|opts
index|[]
init|=
block|{
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"all"
argument_list|,
operator|&
name|flags
argument_list|,
literal|"pack everything"
argument_list|,
name|PACK_REFS_ALL
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"prune"
argument_list|,
operator|&
name|flags
argument_list|,
literal|"prune loose refs (default)"
argument_list|,
name|PACK_REFS_PRUNE
argument_list|)
block|,
name|OPT_END
argument_list|()
block|, 	}
decl_stmt|;
if|if
condition|(
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|opts
argument_list|,
name|pack_refs_usage
argument_list|,
literal|0
argument_list|)
condition|)
name|usage_with_options
argument_list|(
name|pack_refs_usage
argument_list|,
name|opts
argument_list|)
expr_stmt|;
return|return
name|pack_refs
argument_list|(
name|flags
argument_list|)
return|;
block|}
end_function
end_unit
