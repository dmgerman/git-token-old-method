begin_unit
begin_include
include|#
directive|include
file|"../git-compat-util.h"
end_include
begin_function
DECL|function|gitstrtoimax
name|intmax_t
name|gitstrtoimax
parameter_list|(
specifier|const
name|char
modifier|*
name|nptr
parameter_list|,
name|char
modifier|*
modifier|*
name|endptr
parameter_list|,
name|int
name|base
parameter_list|)
block|{
if|#
directive|if
name|defined
argument_list|(
name|NO_STRTOULL
argument_list|)
return|return
name|strtol
argument_list|(
name|nptr
argument_list|,
name|endptr
argument_list|,
name|base
argument_list|)
return|;
else|#
directive|else
return|return
name|strtoll
argument_list|(
name|nptr
argument_list|,
name|endptr
argument_list|,
name|base
argument_list|)
return|;
endif|#
directive|endif
block|}
end_function
end_unit
