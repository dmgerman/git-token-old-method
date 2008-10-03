begin_unit
begin_comment
comment|/*  * SHA-1 implementation optimized for ARM  *  * Copyright:   (C) 2005 by Nicolas Pitre<nico@cam.org>  * Created:     September 17, 2005  */
end_comment
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_include
include|#
directive|include
file|"sha1.h"
end_include
begin_function_decl
specifier|extern
name|void
name|arm_sha_transform
parameter_list|(
name|uint32_t
modifier|*
name|hash
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|data
parameter_list|,
name|uint32_t
modifier|*
name|W
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|arm_SHA1_Init
name|void
name|arm_SHA1_Init
parameter_list|(
name|arm_SHA_CTX
modifier|*
name|c
parameter_list|)
block|{
name|c
operator|->
name|len
operator|=
literal|0
expr_stmt|;
name|c
operator|->
name|hash
index|[
literal|0
index|]
operator|=
literal|0x67452301
expr_stmt|;
name|c
operator|->
name|hash
index|[
literal|1
index|]
operator|=
literal|0xefcdab89
expr_stmt|;
name|c
operator|->
name|hash
index|[
literal|2
index|]
operator|=
literal|0x98badcfe
expr_stmt|;
name|c
operator|->
name|hash
index|[
literal|3
index|]
operator|=
literal|0x10325476
expr_stmt|;
name|c
operator|->
name|hash
index|[
literal|4
index|]
operator|=
literal|0xc3d2e1f0
expr_stmt|;
block|}
end_function
begin_function
DECL|function|arm_SHA1_Update
name|void
name|arm_SHA1_Update
parameter_list|(
name|arm_SHA_CTX
modifier|*
name|c
parameter_list|,
specifier|const
name|void
modifier|*
name|p
parameter_list|,
name|unsigned
name|long
name|n
parameter_list|)
block|{
name|uint32_t
name|workspace
index|[
literal|80
index|]
decl_stmt|;
name|unsigned
name|int
name|partial
decl_stmt|;
name|unsigned
name|long
name|done
decl_stmt|;
name|partial
operator|=
name|c
operator|->
name|len
operator|&
literal|0x3f
expr_stmt|;
name|c
operator|->
name|len
operator|+=
name|n
expr_stmt|;
if|if
condition|(
operator|(
name|partial
operator|+
name|n
operator|)
operator|>=
literal|64
condition|)
block|{
if|if
condition|(
name|partial
condition|)
block|{
name|done
operator|=
literal|64
operator|-
name|partial
expr_stmt|;
name|memcpy
argument_list|(
name|c
operator|->
name|buffer
operator|+
name|partial
argument_list|,
name|p
argument_list|,
name|done
argument_list|)
expr_stmt|;
name|arm_sha_transform
argument_list|(
name|c
operator|->
name|hash
argument_list|,
name|c
operator|->
name|buffer
argument_list|,
name|workspace
argument_list|)
expr_stmt|;
name|partial
operator|=
literal|0
expr_stmt|;
block|}
else|else
name|done
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|n
operator|>=
name|done
operator|+
literal|64
condition|)
block|{
name|arm_sha_transform
argument_list|(
name|c
operator|->
name|hash
argument_list|,
name|p
operator|+
name|done
argument_list|,
name|workspace
argument_list|)
expr_stmt|;
name|done
operator|+=
literal|64
expr_stmt|;
block|}
block|}
else|else
name|done
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|n
operator|-
name|done
condition|)
name|memcpy
argument_list|(
name|c
operator|->
name|buffer
operator|+
name|partial
argument_list|,
name|p
operator|+
name|done
argument_list|,
name|n
operator|-
name|done
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|arm_SHA1_Final
name|void
name|arm_SHA1_Final
parameter_list|(
name|unsigned
name|char
modifier|*
name|hash
parameter_list|,
name|arm_SHA_CTX
modifier|*
name|c
parameter_list|)
block|{
name|uint64_t
name|bitlen
decl_stmt|;
name|uint32_t
name|bitlen_hi
decl_stmt|,
name|bitlen_lo
decl_stmt|;
name|unsigned
name|int
name|i
decl_stmt|,
name|offset
decl_stmt|,
name|padlen
decl_stmt|;
name|unsigned
name|char
name|bits
index|[
literal|8
index|]
decl_stmt|;
specifier|static
specifier|const
name|unsigned
name|char
name|padding
index|[
literal|64
index|]
init|=
block|{
literal|0x80
block|, }
decl_stmt|;
name|bitlen
operator|=
name|c
operator|->
name|len
operator|<<
literal|3
expr_stmt|;
name|offset
operator|=
name|c
operator|->
name|len
operator|&
literal|0x3f
expr_stmt|;
name|padlen
operator|=
operator|(
operator|(
name|offset
operator|<
literal|56
operator|)
condition|?
literal|56
else|:
operator|(
literal|64
operator|+
literal|56
operator|)
operator|)
operator|-
name|offset
expr_stmt|;
name|arm_SHA1_Update
argument_list|(
name|c
argument_list|,
name|padding
argument_list|,
name|padlen
argument_list|)
expr_stmt|;
name|bitlen_hi
operator|=
name|bitlen
operator|>>
literal|32
expr_stmt|;
name|bitlen_lo
operator|=
name|bitlen
operator|&
literal|0xffffffff
expr_stmt|;
name|bits
index|[
literal|0
index|]
operator|=
name|bitlen_hi
operator|>>
literal|24
expr_stmt|;
name|bits
index|[
literal|1
index|]
operator|=
name|bitlen_hi
operator|>>
literal|16
expr_stmt|;
name|bits
index|[
literal|2
index|]
operator|=
name|bitlen_hi
operator|>>
literal|8
expr_stmt|;
name|bits
index|[
literal|3
index|]
operator|=
name|bitlen_hi
expr_stmt|;
name|bits
index|[
literal|4
index|]
operator|=
name|bitlen_lo
operator|>>
literal|24
expr_stmt|;
name|bits
index|[
literal|5
index|]
operator|=
name|bitlen_lo
operator|>>
literal|16
expr_stmt|;
name|bits
index|[
literal|6
index|]
operator|=
name|bitlen_lo
operator|>>
literal|8
expr_stmt|;
name|bits
index|[
literal|7
index|]
operator|=
name|bitlen_lo
expr_stmt|;
name|arm_SHA1_Update
argument_list|(
name|c
argument_list|,
name|bits
argument_list|,
literal|8
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
literal|5
condition|;
name|i
operator|++
control|)
block|{
name|uint32_t
name|v
init|=
name|c
operator|->
name|hash
index|[
name|i
index|]
decl_stmt|;
name|hash
index|[
literal|0
index|]
operator|=
name|v
operator|>>
literal|24
expr_stmt|;
name|hash
index|[
literal|1
index|]
operator|=
name|v
operator|>>
literal|16
expr_stmt|;
name|hash
index|[
literal|2
index|]
operator|=
name|v
operator|>>
literal|8
expr_stmt|;
name|hash
index|[
literal|3
index|]
operator|=
name|v
expr_stmt|;
name|hash
operator|+=
literal|4
expr_stmt|;
block|}
block|}
end_function
end_unit
