begin_unit
begin_comment
comment|/*  * Copyright (C) 2005 Junio C Hamano  * The delta-parsing part is almost straight copy of patch-delta.c  * which is (C) 2005 Nicolas Pitre<nico@cam.org>.  */
end_comment
begin_include
include|#
directive|include
file|<stdlib.h>
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_include
include|#
directive|include
file|<limits.h>
end_include
begin_include
include|#
directive|include
file|"count-delta.h"
end_include
begin_function
DECL|function|get_hdr_size
specifier|static
name|unsigned
name|long
name|get_hdr_size
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
modifier|*
name|datap
parameter_list|)
block|{
specifier|const
name|unsigned
name|char
modifier|*
name|data
init|=
operator|*
name|datap
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|unsigned
name|char
name|cmd
decl_stmt|;
name|int
name|i
decl_stmt|;
name|size
operator|=
name|i
operator|=
literal|0
expr_stmt|;
name|cmd
operator|=
operator|*
name|data
operator|++
expr_stmt|;
while|while
condition|(
name|cmd
condition|)
block|{
if|if
condition|(
name|cmd
operator|&
literal|1
condition|)
name|size
operator||=
operator|*
name|data
operator|++
operator|<<
name|i
expr_stmt|;
name|i
operator|+=
literal|8
expr_stmt|;
name|cmd
operator|>>=
literal|1
expr_stmt|;
block|}
operator|*
name|datap
operator|=
name|data
expr_stmt|;
return|return
name|size
return|;
block|}
end_function
begin_comment
comment|/*  * NOTE.  We do not _interpret_ delta fully.  As an approximation, we  * just count the number of bytes that are copied from the source, and  * the number of literal data bytes that are inserted.  Number of  * bytes that are _not_ copied from the source is deletion, and number  * of inserted literal bytes are addition, so sum of them is what we  * return.  xdelta can express an edit that copies data inside of the  * destination which originally came from the source.  We do not count  * that in the following routine, so we are undercounting the source  * material that remains in the final output that way.  */
end_comment
begin_function
DECL|function|count_delta
name|unsigned
name|long
name|count_delta
parameter_list|(
name|void
modifier|*
name|delta_buf
parameter_list|,
name|unsigned
name|long
name|delta_size
parameter_list|)
block|{
name|unsigned
name|long
name|copied_from_source
decl_stmt|,
name|added_literal
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|data
decl_stmt|,
modifier|*
name|top
decl_stmt|;
name|unsigned
name|char
name|cmd
decl_stmt|;
name|unsigned
name|long
name|src_size
decl_stmt|,
name|dst_size
decl_stmt|,
name|out
decl_stmt|;
comment|/* the smallest delta size possible is 6 bytes */
if|if
condition|(
name|delta_size
operator|<
literal|6
condition|)
return|return
name|UINT_MAX
return|;
name|data
operator|=
name|delta_buf
expr_stmt|;
name|top
operator|=
name|delta_buf
operator|+
name|delta_size
expr_stmt|;
name|src_size
operator|=
name|get_hdr_size
argument_list|(
operator|&
name|data
argument_list|)
expr_stmt|;
name|dst_size
operator|=
name|get_hdr_size
argument_list|(
operator|&
name|data
argument_list|)
expr_stmt|;
name|added_literal
operator|=
name|copied_from_source
operator|=
name|out
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|data
operator|<
name|top
condition|)
block|{
name|cmd
operator|=
operator|*
name|data
operator|++
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x80
condition|)
block|{
name|unsigned
name|long
name|cp_off
init|=
literal|0
decl_stmt|,
name|cp_size
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x01
condition|)
name|cp_off
operator|=
operator|*
name|data
operator|++
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x02
condition|)
name|cp_off
operator||=
operator|(
operator|*
name|data
operator|++
operator|<<
literal|8
operator|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x04
condition|)
name|cp_off
operator||=
operator|(
operator|*
name|data
operator|++
operator|<<
literal|16
operator|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x08
condition|)
name|cp_off
operator||=
operator|(
operator|*
name|data
operator|++
operator|<<
literal|24
operator|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x10
condition|)
name|cp_size
operator|=
operator|*
name|data
operator|++
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x20
condition|)
name|cp_size
operator||=
operator|(
operator|*
name|data
operator|++
operator|<<
literal|8
operator|)
expr_stmt|;
if|if
condition|(
name|cp_size
operator|==
literal|0
condition|)
name|cp_size
operator|=
literal|0x10000
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x40
condition|)
comment|/* copy from dst */
empty_stmt|;
else|else
name|copied_from_source
operator|+=
name|cp_size
expr_stmt|;
name|out
operator|+=
name|cp_size
expr_stmt|;
block|}
else|else
block|{
comment|/* write literal into dst */
name|added_literal
operator|+=
name|cmd
expr_stmt|;
name|out
operator|+=
name|cmd
expr_stmt|;
name|data
operator|+=
name|cmd
expr_stmt|;
block|}
block|}
comment|/* sanity check */
if|if
condition|(
name|data
operator|!=
name|top
operator|||
name|out
operator|!=
name|dst_size
condition|)
return|return
name|UINT_MAX
return|;
comment|/* delete size is what was _not_ copied from source. 	 * edit size is that and literal additions. 	 */
return|return
operator|(
name|src_size
operator|-
name|copied_from_source
operator|)
operator|+
name|added_literal
return|;
block|}
end_function
end_unit
