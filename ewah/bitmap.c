begin_unit
begin_comment
comment|/**  * Copyright 2013, GitHub, Inc  * Copyright 2009-2013, Daniel Lemire, Cliff Moon,  *	David McIntosh, Robert Becho, Google Inc. and Veronika Zenz  *  * This program is free software; you can redistribute it and/or  * modify it under the terms of the GNU General Public License  * as published by the Free Software Foundation; either version 2  * of the License, or (at your option) any later version.  *  * This program is distributed in the hope that it will be useful,  * but WITHOUT ANY WARRANTY; without even the implied warranty of  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  * GNU General Public License for more details.  *  * You should have received a copy of the GNU General Public License  * along with this program; if not, write to the Free Software  * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"ewok.h"
end_include
begin_define
DECL|macro|EWAH_MASK
define|#
directive|define
name|EWAH_MASK
parameter_list|(
name|x
parameter_list|)
value|((eword_t)1<< (x % BITS_IN_EWORD))
end_define
begin_define
DECL|macro|EWAH_BLOCK
define|#
directive|define
name|EWAH_BLOCK
parameter_list|(
name|x
parameter_list|)
value|(x / BITS_IN_EWORD)
end_define
begin_function
DECL|function|bitmap_new
name|struct
name|bitmap
modifier|*
name|bitmap_new
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|bitmap
modifier|*
name|bitmap
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|bitmap
argument_list|)
argument_list|)
decl_stmt|;
name|bitmap
operator|->
name|words
operator|=
name|xcalloc
argument_list|(
literal|32
argument_list|,
sizeof|sizeof
argument_list|(
name|eword_t
argument_list|)
argument_list|)
expr_stmt|;
name|bitmap
operator|->
name|word_alloc
operator|=
literal|32
expr_stmt|;
return|return
name|bitmap
return|;
block|}
end_function
begin_function
DECL|function|bitmap_set
name|void
name|bitmap_set
parameter_list|(
name|struct
name|bitmap
modifier|*
name|self
parameter_list|,
name|size_t
name|pos
parameter_list|)
block|{
name|size_t
name|block
init|=
name|EWAH_BLOCK
argument_list|(
name|pos
argument_list|)
decl_stmt|;
if|if
condition|(
name|block
operator|>=
name|self
operator|->
name|word_alloc
condition|)
block|{
name|size_t
name|old_size
init|=
name|self
operator|->
name|word_alloc
decl_stmt|;
name|self
operator|->
name|word_alloc
operator|=
name|block
operator|*
literal|2
expr_stmt|;
name|REALLOC_ARRAY
argument_list|(
name|self
operator|->
name|words
argument_list|,
name|self
operator|->
name|word_alloc
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|self
operator|->
name|words
operator|+
name|old_size
argument_list|,
literal|0x0
argument_list|,
operator|(
name|self
operator|->
name|word_alloc
operator|-
name|old_size
operator|)
operator|*
sizeof|sizeof
argument_list|(
name|eword_t
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|self
operator|->
name|words
index|[
name|block
index|]
operator||=
name|EWAH_MASK
argument_list|(
name|pos
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|bitmap_clear
name|void
name|bitmap_clear
parameter_list|(
name|struct
name|bitmap
modifier|*
name|self
parameter_list|,
name|size_t
name|pos
parameter_list|)
block|{
name|size_t
name|block
init|=
name|EWAH_BLOCK
argument_list|(
name|pos
argument_list|)
decl_stmt|;
if|if
condition|(
name|block
operator|<
name|self
operator|->
name|word_alloc
condition|)
name|self
operator|->
name|words
index|[
name|block
index|]
operator|&=
operator|~
name|EWAH_MASK
argument_list|(
name|pos
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|bitmap_get
name|int
name|bitmap_get
parameter_list|(
name|struct
name|bitmap
modifier|*
name|self
parameter_list|,
name|size_t
name|pos
parameter_list|)
block|{
name|size_t
name|block
init|=
name|EWAH_BLOCK
argument_list|(
name|pos
argument_list|)
decl_stmt|;
return|return
name|block
operator|<
name|self
operator|->
name|word_alloc
operator|&&
operator|(
name|self
operator|->
name|words
index|[
name|block
index|]
operator|&
name|EWAH_MASK
argument_list|(
name|pos
argument_list|)
operator|)
operator|!=
literal|0
return|;
block|}
end_function
begin_function
DECL|function|bitmap_to_ewah
name|struct
name|ewah_bitmap
modifier|*
name|bitmap_to_ewah
parameter_list|(
name|struct
name|bitmap
modifier|*
name|bitmap
parameter_list|)
block|{
name|struct
name|ewah_bitmap
modifier|*
name|ewah
init|=
name|ewah_new
argument_list|()
decl_stmt|;
name|size_t
name|i
decl_stmt|,
name|running_empty_words
init|=
literal|0
decl_stmt|;
name|eword_t
name|last_word
init|=
literal|0
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|bitmap
operator|->
name|word_alloc
condition|;
operator|++
name|i
control|)
block|{
if|if
condition|(
name|bitmap
operator|->
name|words
index|[
name|i
index|]
operator|==
literal|0
condition|)
block|{
name|running_empty_words
operator|++
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|last_word
operator|!=
literal|0
condition|)
name|ewah_add
argument_list|(
name|ewah
argument_list|,
name|last_word
argument_list|)
expr_stmt|;
if|if
condition|(
name|running_empty_words
operator|>
literal|0
condition|)
block|{
name|ewah_add_empty_words
argument_list|(
name|ewah
argument_list|,
literal|0
argument_list|,
name|running_empty_words
argument_list|)
expr_stmt|;
name|running_empty_words
operator|=
literal|0
expr_stmt|;
block|}
name|last_word
operator|=
name|bitmap
operator|->
name|words
index|[
name|i
index|]
expr_stmt|;
block|}
name|ewah_add
argument_list|(
name|ewah
argument_list|,
name|last_word
argument_list|)
expr_stmt|;
return|return
name|ewah
return|;
block|}
end_function
begin_function
DECL|function|ewah_to_bitmap
name|struct
name|bitmap
modifier|*
name|ewah_to_bitmap
parameter_list|(
name|struct
name|ewah_bitmap
modifier|*
name|ewah
parameter_list|)
block|{
name|struct
name|bitmap
modifier|*
name|bitmap
init|=
name|bitmap_new
argument_list|()
decl_stmt|;
name|struct
name|ewah_iterator
name|it
decl_stmt|;
name|eword_t
name|blowup
decl_stmt|;
name|size_t
name|i
init|=
literal|0
decl_stmt|;
name|ewah_iterator_init
argument_list|(
operator|&
name|it
argument_list|,
name|ewah
argument_list|)
expr_stmt|;
while|while
condition|(
name|ewah_iterator_next
argument_list|(
operator|&
name|blowup
argument_list|,
operator|&
name|it
argument_list|)
condition|)
block|{
name|ALLOC_GROW
argument_list|(
name|bitmap
operator|->
name|words
argument_list|,
name|i
operator|+
literal|1
argument_list|,
name|bitmap
operator|->
name|word_alloc
argument_list|)
expr_stmt|;
name|bitmap
operator|->
name|words
index|[
name|i
operator|++
index|]
operator|=
name|blowup
expr_stmt|;
block|}
name|bitmap
operator|->
name|word_alloc
operator|=
name|i
expr_stmt|;
return|return
name|bitmap
return|;
block|}
end_function
begin_function
DECL|function|bitmap_and_not
name|void
name|bitmap_and_not
parameter_list|(
name|struct
name|bitmap
modifier|*
name|self
parameter_list|,
name|struct
name|bitmap
modifier|*
name|other
parameter_list|)
block|{
specifier|const
name|size_t
name|count
init|=
operator|(
name|self
operator|->
name|word_alloc
operator|<
name|other
operator|->
name|word_alloc
operator|)
condition|?
name|self
operator|->
name|word_alloc
else|:
name|other
operator|->
name|word_alloc
decl_stmt|;
name|size_t
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|count
condition|;
operator|++
name|i
control|)
name|self
operator|->
name|words
index|[
name|i
index|]
operator|&=
operator|~
name|other
operator|->
name|words
index|[
name|i
index|]
expr_stmt|;
block|}
end_function
begin_function
DECL|function|bitmap_or_ewah
name|void
name|bitmap_or_ewah
parameter_list|(
name|struct
name|bitmap
modifier|*
name|self
parameter_list|,
name|struct
name|ewah_bitmap
modifier|*
name|other
parameter_list|)
block|{
name|size_t
name|original_size
init|=
name|self
operator|->
name|word_alloc
decl_stmt|;
name|size_t
name|other_final
init|=
operator|(
name|other
operator|->
name|bit_size
operator|/
name|BITS_IN_EWORD
operator|)
operator|+
literal|1
decl_stmt|;
name|size_t
name|i
init|=
literal|0
decl_stmt|;
name|struct
name|ewah_iterator
name|it
decl_stmt|;
name|eword_t
name|word
decl_stmt|;
if|if
condition|(
name|self
operator|->
name|word_alloc
operator|<
name|other_final
condition|)
block|{
name|self
operator|->
name|word_alloc
operator|=
name|other_final
expr_stmt|;
name|REALLOC_ARRAY
argument_list|(
name|self
operator|->
name|words
argument_list|,
name|self
operator|->
name|word_alloc
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|self
operator|->
name|words
operator|+
name|original_size
argument_list|,
literal|0x0
argument_list|,
operator|(
name|self
operator|->
name|word_alloc
operator|-
name|original_size
operator|)
operator|*
sizeof|sizeof
argument_list|(
name|eword_t
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|ewah_iterator_init
argument_list|(
operator|&
name|it
argument_list|,
name|other
argument_list|)
expr_stmt|;
while|while
condition|(
name|ewah_iterator_next
argument_list|(
operator|&
name|word
argument_list|,
operator|&
name|it
argument_list|)
condition|)
name|self
operator|->
name|words
index|[
name|i
operator|++
index|]
operator||=
name|word
expr_stmt|;
block|}
end_function
begin_function
DECL|function|bitmap_each_bit
name|void
name|bitmap_each_bit
parameter_list|(
name|struct
name|bitmap
modifier|*
name|self
parameter_list|,
name|ewah_callback
name|callback
parameter_list|,
name|void
modifier|*
name|data
parameter_list|)
block|{
name|size_t
name|pos
init|=
literal|0
decl_stmt|,
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|self
operator|->
name|word_alloc
condition|;
operator|++
name|i
control|)
block|{
name|eword_t
name|word
init|=
name|self
operator|->
name|words
index|[
name|i
index|]
decl_stmt|;
name|uint32_t
name|offset
decl_stmt|;
if|if
condition|(
name|word
operator|==
operator|(
name|eword_t
operator|)
operator|~
literal|0
condition|)
block|{
for|for
control|(
name|offset
operator|=
literal|0
init|;
name|offset
operator|<
name|BITS_IN_EWORD
condition|;
operator|++
name|offset
control|)
name|callback
argument_list|(
name|pos
operator|++
argument_list|,
name|data
argument_list|)
expr_stmt|;
block|}
else|else
block|{
for|for
control|(
name|offset
operator|=
literal|0
init|;
name|offset
operator|<
name|BITS_IN_EWORD
condition|;
operator|++
name|offset
control|)
block|{
if|if
condition|(
operator|(
name|word
operator|>>
name|offset
operator|)
operator|==
literal|0
condition|)
break|break;
name|offset
operator|+=
name|ewah_bit_ctz64
argument_list|(
name|word
operator|>>
name|offset
argument_list|)
expr_stmt|;
name|callback
argument_list|(
name|pos
operator|+
name|offset
argument_list|,
name|data
argument_list|)
expr_stmt|;
block|}
name|pos
operator|+=
name|BITS_IN_EWORD
expr_stmt|;
block|}
block|}
block|}
end_function
begin_function
DECL|function|bitmap_popcount
name|size_t
name|bitmap_popcount
parameter_list|(
name|struct
name|bitmap
modifier|*
name|self
parameter_list|)
block|{
name|size_t
name|i
decl_stmt|,
name|count
init|=
literal|0
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|self
operator|->
name|word_alloc
condition|;
operator|++
name|i
control|)
name|count
operator|+=
name|ewah_bit_popcount64
argument_list|(
name|self
operator|->
name|words
index|[
name|i
index|]
argument_list|)
expr_stmt|;
return|return
name|count
return|;
block|}
end_function
begin_function
DECL|function|bitmap_equals
name|int
name|bitmap_equals
parameter_list|(
name|struct
name|bitmap
modifier|*
name|self
parameter_list|,
name|struct
name|bitmap
modifier|*
name|other
parameter_list|)
block|{
name|struct
name|bitmap
modifier|*
name|big
decl_stmt|,
modifier|*
name|small
decl_stmt|;
name|size_t
name|i
decl_stmt|;
if|if
condition|(
name|self
operator|->
name|word_alloc
operator|<
name|other
operator|->
name|word_alloc
condition|)
block|{
name|small
operator|=
name|self
expr_stmt|;
name|big
operator|=
name|other
expr_stmt|;
block|}
else|else
block|{
name|small
operator|=
name|other
expr_stmt|;
name|big
operator|=
name|self
expr_stmt|;
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|small
operator|->
name|word_alloc
condition|;
operator|++
name|i
control|)
block|{
if|if
condition|(
name|small
operator|->
name|words
index|[
name|i
index|]
operator|!=
name|big
operator|->
name|words
index|[
name|i
index|]
condition|)
return|return
literal|0
return|;
block|}
for|for
control|(
init|;
name|i
operator|<
name|big
operator|->
name|word_alloc
condition|;
operator|++
name|i
control|)
block|{
if|if
condition|(
name|big
operator|->
name|words
index|[
name|i
index|]
operator|!=
literal|0
condition|)
return|return
literal|0
return|;
block|}
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|bitmap_reset
name|void
name|bitmap_reset
parameter_list|(
name|struct
name|bitmap
modifier|*
name|bitmap
parameter_list|)
block|{
name|memset
argument_list|(
name|bitmap
operator|->
name|words
argument_list|,
literal|0x0
argument_list|,
name|bitmap
operator|->
name|word_alloc
operator|*
sizeof|sizeof
argument_list|(
name|eword_t
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|bitmap_free
name|void
name|bitmap_free
parameter_list|(
name|struct
name|bitmap
modifier|*
name|bitmap
parameter_list|)
block|{
if|if
condition|(
name|bitmap
operator|==
name|NULL
condition|)
return|return;
name|free
argument_list|(
name|bitmap
operator|->
name|words
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|bitmap
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
