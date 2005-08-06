begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"pkt-line.h"
end_include
begin_include
include|#
directive|include
file|"quote.h"
end_include
begin_include
include|#
directive|include
file|<sys/wait.h>
end_include
begin_include
include|#
directive|include
file|<sys/socket.h>
end_include
begin_include
include|#
directive|include
file|<netinet/in.h>
end_include
begin_include
include|#
directive|include
file|<arpa/inet.h>
end_include
begin_include
include|#
directive|include
file|<netdb.h>
end_include
begin_comment
comment|/*  * Read all the refs from the other end  */
end_comment
begin_function
DECL|function|get_remote_heads
name|struct
name|ref
modifier|*
modifier|*
name|get_remote_heads
parameter_list|(
name|int
name|in
parameter_list|,
name|struct
name|ref
modifier|*
modifier|*
name|list
parameter_list|,
name|int
name|nr_match
parameter_list|,
name|char
modifier|*
modifier|*
name|match
parameter_list|)
block|{
operator|*
name|list
operator|=
name|NULL
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|struct
name|ref
modifier|*
name|ref
decl_stmt|;
name|unsigned
name|char
name|old_sha1
index|[
literal|20
index|]
decl_stmt|;
specifier|static
name|char
name|buffer
index|[
literal|1000
index|]
decl_stmt|;
name|char
modifier|*
name|name
decl_stmt|;
name|int
name|len
decl_stmt|;
name|len
operator|=
name|packet_read_line
argument_list|(
name|in
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
operator|!
name|len
condition|)
break|break;
if|if
condition|(
name|buffer
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\n'
condition|)
name|buffer
index|[
operator|--
name|len
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|len
operator|<
literal|42
operator|||
name|get_sha1_hex
argument_list|(
name|buffer
argument_list|,
name|old_sha1
argument_list|)
operator|||
name|buffer
index|[
literal|40
index|]
operator|!=
literal|' '
condition|)
name|die
argument_list|(
literal|"protocol error: expected sha/ref, got '%s'"
argument_list|,
name|buffer
argument_list|)
expr_stmt|;
name|name
operator|=
name|buffer
operator|+
literal|41
expr_stmt|;
if|if
condition|(
name|nr_match
operator|&&
operator|!
name|path_match
argument_list|(
name|name
argument_list|,
name|nr_match
argument_list|,
name|match
argument_list|)
condition|)
continue|continue;
name|ref
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|ref
argument_list|)
operator|+
name|len
operator|-
literal|40
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|ref
operator|->
name|old_sha1
argument_list|,
name|old_sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|ref
operator|->
name|name
argument_list|,
name|buffer
operator|+
literal|41
argument_list|,
name|len
operator|-
literal|40
argument_list|)
expr_stmt|;
operator|*
name|list
operator|=
name|ref
expr_stmt|;
name|list
operator|=
operator|&
name|ref
operator|->
name|next
expr_stmt|;
block|}
return|return
name|list
return|;
block|}
end_function
begin_function
DECL|function|get_ack
name|int
name|get_ack
parameter_list|(
name|int
name|fd
parameter_list|,
name|unsigned
name|char
modifier|*
name|result_sha1
parameter_list|)
block|{
specifier|static
name|char
name|line
index|[
literal|1000
index|]
decl_stmt|;
name|int
name|len
init|=
name|packet_read_line
argument_list|(
name|fd
argument_list|,
name|line
argument_list|,
sizeof|sizeof
argument_list|(
name|line
argument_list|)
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|len
condition|)
name|die
argument_list|(
literal|"git-fetch-pack: expected ACK/NAK, got EOF"
argument_list|)
expr_stmt|;
if|if
condition|(
name|line
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\n'
condition|)
name|line
index|[
operator|--
name|len
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|line
argument_list|,
literal|"NAK"
argument_list|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|line
argument_list|,
literal|"ACK "
argument_list|,
literal|3
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|get_sha1_hex
argument_list|(
name|line
operator|+
literal|4
argument_list|,
name|result_sha1
argument_list|)
condition|)
return|return
literal|1
return|;
block|}
name|die
argument_list|(
literal|"git-fetch_pack: expected ACK/NAK, got '%s'"
argument_list|,
name|line
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|path_match
name|int
name|path_match
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|nr
parameter_list|,
name|char
modifier|*
modifier|*
name|match
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|int
name|pathlen
init|=
name|strlen
argument_list|(
name|path
argument_list|)
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|char
modifier|*
name|s
init|=
name|match
index|[
name|i
index|]
decl_stmt|;
name|int
name|len
init|=
name|strlen
argument_list|(
name|s
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|len
operator|||
name|len
operator|>
name|pathlen
condition|)
continue|continue;
if|if
condition|(
name|memcmp
argument_list|(
name|path
operator|+
name|pathlen
operator|-
name|len
argument_list|,
name|s
argument_list|,
name|len
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|pathlen
operator|>
name|len
operator|&&
name|path
index|[
name|pathlen
operator|-
name|len
operator|-
literal|1
index|]
operator|!=
literal|'/'
condition|)
continue|continue;
operator|*
name|s
operator|=
literal|0
expr_stmt|;
return|return
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_struct
DECL|struct|refspec
struct|struct
name|refspec
block|{
DECL|member|src
name|char
modifier|*
name|src
decl_stmt|;
DECL|member|dst
name|char
modifier|*
name|dst
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|parse_ref_spec
specifier|static
name|struct
name|refspec
modifier|*
name|parse_ref_spec
parameter_list|(
name|int
name|nr_refspec
parameter_list|,
name|char
modifier|*
modifier|*
name|refspec
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|struct
name|refspec
modifier|*
name|rs
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|rs
argument_list|)
operator|*
operator|(
name|nr_refspec
operator|+
literal|1
operator|)
argument_list|)
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_refspec
condition|;
name|i
operator|++
control|)
block|{
name|char
modifier|*
name|sp
decl_stmt|,
modifier|*
name|dp
decl_stmt|,
modifier|*
name|ep
decl_stmt|;
name|sp
operator|=
name|refspec
index|[
name|i
index|]
expr_stmt|;
name|ep
operator|=
name|strchr
argument_list|(
name|sp
argument_list|,
literal|':'
argument_list|)
expr_stmt|;
if|if
condition|(
name|ep
condition|)
block|{
name|dp
operator|=
name|ep
operator|+
literal|1
expr_stmt|;
operator|*
name|ep
operator|=
literal|0
expr_stmt|;
block|}
else|else
name|dp
operator|=
name|sp
expr_stmt|;
name|rs
index|[
name|i
index|]
operator|.
name|src
operator|=
name|sp
expr_stmt|;
name|rs
index|[
name|i
index|]
operator|.
name|dst
operator|=
name|dp
expr_stmt|;
block|}
name|rs
index|[
name|nr_refspec
index|]
operator|.
name|src
operator|=
name|rs
index|[
name|nr_refspec
index|]
operator|.
name|dst
operator|=
name|NULL
expr_stmt|;
return|return
name|rs
return|;
block|}
end_function
begin_function
DECL|function|count_refspec_match
specifier|static
name|int
name|count_refspec_match
parameter_list|(
specifier|const
name|char
modifier|*
name|pattern
parameter_list|,
name|struct
name|ref
modifier|*
name|refs
parameter_list|,
name|struct
name|ref
modifier|*
modifier|*
name|matched_ref
parameter_list|)
block|{
name|int
name|match
decl_stmt|;
name|int
name|patlen
init|=
name|strlen
argument_list|(
name|pattern
argument_list|)
decl_stmt|;
for|for
control|(
name|match
operator|=
literal|0
init|;
name|refs
condition|;
name|refs
operator|=
name|refs
operator|->
name|next
control|)
block|{
name|char
modifier|*
name|name
init|=
name|refs
operator|->
name|name
decl_stmt|;
name|int
name|namelen
init|=
name|strlen
argument_list|(
name|name
argument_list|)
decl_stmt|;
if|if
condition|(
name|namelen
operator|<
name|patlen
operator|||
name|memcmp
argument_list|(
name|name
operator|+
name|namelen
operator|-
name|patlen
argument_list|,
name|pattern
argument_list|,
name|patlen
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|namelen
operator|!=
name|patlen
operator|&&
name|name
index|[
name|namelen
operator|-
name|patlen
operator|-
literal|1
index|]
operator|!=
literal|'/'
condition|)
continue|continue;
name|match
operator|++
expr_stmt|;
operator|*
name|matched_ref
operator|=
name|refs
expr_stmt|;
block|}
return|return
name|match
return|;
block|}
end_function
begin_function
DECL|function|link_dst_tail
specifier|static
name|void
name|link_dst_tail
parameter_list|(
name|struct
name|ref
modifier|*
name|ref
parameter_list|,
name|struct
name|ref
modifier|*
modifier|*
modifier|*
name|tail
parameter_list|)
block|{
operator|*
operator|*
name|tail
operator|=
name|ref
expr_stmt|;
operator|*
name|tail
operator|=
operator|&
name|ref
operator|->
name|next
expr_stmt|;
operator|*
operator|*
name|tail
operator|=
name|NULL
expr_stmt|;
block|}
end_function
begin_function
DECL|function|try_explicit_object_name
specifier|static
name|struct
name|ref
modifier|*
name|try_explicit_object_name
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|ref
modifier|*
name|ref
decl_stmt|;
name|int
name|len
decl_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|name
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
name|NULL
return|;
name|len
operator|=
name|strlen
argument_list|(
name|name
argument_list|)
operator|+
literal|1
expr_stmt|;
name|ref
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|ref
argument_list|)
operator|+
name|len
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|ref
operator|->
name|name
argument_list|,
name|name
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|ref
operator|->
name|new_sha1
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
name|ref
return|;
block|}
end_function
begin_function
DECL|function|match_explicit_refs
specifier|static
name|int
name|match_explicit_refs
parameter_list|(
name|struct
name|ref
modifier|*
name|src
parameter_list|,
name|struct
name|ref
modifier|*
name|dst
parameter_list|,
name|struct
name|ref
modifier|*
modifier|*
modifier|*
name|dst_tail
parameter_list|,
name|struct
name|refspec
modifier|*
name|rs
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|errs
decl_stmt|;
for|for
control|(
name|i
operator|=
name|errs
operator|=
literal|0
init|;
name|rs
index|[
name|i
index|]
operator|.
name|src
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|ref
modifier|*
name|matched_src
decl_stmt|,
modifier|*
name|matched_dst
decl_stmt|;
name|matched_src
operator|=
name|matched_dst
operator|=
name|NULL
expr_stmt|;
switch|switch
condition|(
name|count_refspec_match
argument_list|(
name|rs
index|[
name|i
index|]
operator|.
name|src
argument_list|,
name|src
argument_list|,
operator|&
name|matched_src
argument_list|)
condition|)
block|{
case|case
literal|1
case|:
break|break;
case|case
literal|0
case|:
comment|/* The source could be in the get_sha1() format 			 * not a reference name. 			 */
name|matched_src
operator|=
name|try_explicit_object_name
argument_list|(
name|rs
index|[
name|i
index|]
operator|.
name|src
argument_list|)
expr_stmt|;
if|if
condition|(
name|matched_src
condition|)
break|break;
name|errs
operator|=
literal|1
expr_stmt|;
name|error
argument_list|(
literal|"src refspec %s does not match any."
argument_list|)
expr_stmt|;
break|break;
default|default:
name|errs
operator|=
literal|1
expr_stmt|;
name|error
argument_list|(
literal|"src refspec %s matches more than one."
argument_list|,
name|rs
index|[
name|i
index|]
operator|.
name|src
argument_list|)
expr_stmt|;
break|break;
block|}
switch|switch
condition|(
name|count_refspec_match
argument_list|(
name|rs
index|[
name|i
index|]
operator|.
name|dst
argument_list|,
name|dst
argument_list|,
operator|&
name|matched_dst
argument_list|)
condition|)
block|{
case|case
literal|1
case|:
break|break;
case|case
literal|0
case|:
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|rs
index|[
name|i
index|]
operator|.
name|dst
argument_list|,
literal|"refs/"
argument_list|,
literal|5
argument_list|)
condition|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|rs
index|[
name|i
index|]
operator|.
name|dst
argument_list|)
operator|+
literal|1
decl_stmt|;
name|matched_dst
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|dst
argument_list|)
operator|+
name|len
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|matched_dst
operator|->
name|name
argument_list|,
name|rs
index|[
name|i
index|]
operator|.
name|dst
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|link_dst_tail
argument_list|(
name|matched_dst
argument_list|,
name|dst_tail
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|rs
index|[
name|i
index|]
operator|.
name|src
argument_list|,
name|rs
index|[
name|i
index|]
operator|.
name|dst
argument_list|)
operator|&&
name|matched_src
condition|)
block|{
comment|/* pushing "master:master" when 				 * remote does not have master yet. 				 */
name|int
name|len
init|=
name|strlen
argument_list|(
name|matched_src
operator|->
name|name
argument_list|)
operator|+
literal|1
decl_stmt|;
name|matched_dst
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|dst
argument_list|)
operator|+
name|len
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|matched_dst
operator|->
name|name
argument_list|,
name|matched_src
operator|->
name|name
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|link_dst_tail
argument_list|(
name|matched_dst
argument_list|,
name|dst_tail
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|errs
operator|=
literal|1
expr_stmt|;
name|error
argument_list|(
literal|"dst refspec %s does not match any "
literal|"existing ref on the remote and does "
literal|"not start with refs/."
argument_list|,
name|rs
index|[
name|i
index|]
operator|.
name|dst
argument_list|)
expr_stmt|;
block|}
break|break;
default|default:
name|errs
operator|=
literal|1
expr_stmt|;
name|error
argument_list|(
literal|"dst refspec %s matches more than one."
argument_list|,
name|rs
index|[
name|i
index|]
operator|.
name|dst
argument_list|)
expr_stmt|;
break|break;
block|}
if|if
condition|(
name|errs
condition|)
continue|continue;
if|if
condition|(
name|matched_dst
operator|->
name|peer_ref
condition|)
block|{
name|errs
operator|=
literal|1
expr_stmt|;
name|error
argument_list|(
literal|"dst ref %s receives from more than one src."
argument_list|,
name|matched_dst
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
else|else
name|matched_dst
operator|->
name|peer_ref
operator|=
name|matched_src
expr_stmt|;
block|}
return|return
operator|-
name|errs
return|;
block|}
end_function
begin_function
DECL|function|find_ref_by_name
specifier|static
name|struct
name|ref
modifier|*
name|find_ref_by_name
parameter_list|(
name|struct
name|ref
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
for|for
control|(
init|;
name|list
condition|;
name|list
operator|=
name|list
operator|->
name|next
control|)
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|list
operator|->
name|name
argument_list|,
name|name
argument_list|)
condition|)
return|return
name|list
return|;
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|match_refs
name|int
name|match_refs
parameter_list|(
name|struct
name|ref
modifier|*
name|src
parameter_list|,
name|struct
name|ref
modifier|*
name|dst
parameter_list|,
name|struct
name|ref
modifier|*
modifier|*
modifier|*
name|dst_tail
parameter_list|,
name|int
name|nr_refspec
parameter_list|,
name|char
modifier|*
modifier|*
name|refspec
parameter_list|,
name|int
name|all
parameter_list|)
block|{
name|struct
name|refspec
modifier|*
name|rs
init|=
name|parse_ref_spec
argument_list|(
name|nr_refspec
argument_list|,
name|refspec
argument_list|)
decl_stmt|;
if|if
condition|(
name|nr_refspec
condition|)
return|return
name|match_explicit_refs
argument_list|(
name|src
argument_list|,
name|dst
argument_list|,
name|dst_tail
argument_list|,
name|rs
argument_list|)
return|;
comment|/* pick the remainder */
for|for
control|(
init|;
name|src
condition|;
name|src
operator|=
name|src
operator|->
name|next
control|)
block|{
name|struct
name|ref
modifier|*
name|dst_peer
decl_stmt|;
if|if
condition|(
name|src
operator|->
name|peer_ref
condition|)
continue|continue;
name|dst_peer
operator|=
name|find_ref_by_name
argument_list|(
name|dst
argument_list|,
name|src
operator|->
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|dst_peer
operator|&&
name|dst_peer
operator|->
name|peer_ref
operator|)
operator|||
operator|(
operator|!
name|dst_peer
operator|&&
operator|!
name|all
operator|)
condition|)
continue|continue;
if|if
condition|(
operator|!
name|dst_peer
condition|)
block|{
comment|/* Create a new one and link it */
name|int
name|len
init|=
name|strlen
argument_list|(
name|src
operator|->
name|name
argument_list|)
operator|+
literal|1
decl_stmt|;
name|dst_peer
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|dst_peer
argument_list|)
operator|+
name|len
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|dst_peer
operator|->
name|name
argument_list|,
name|src
operator|->
name|name
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|dst_peer
operator|->
name|new_sha1
argument_list|,
name|src
operator|->
name|new_sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|link_dst_tail
argument_list|(
name|dst_peer
argument_list|,
name|dst_tail
argument_list|)
expr_stmt|;
block|}
name|dst_peer
operator|->
name|peer_ref
operator|=
name|src
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_enum
DECL|enum|protocol
enum|enum
name|protocol
block|{
DECL|enumerator|PROTO_LOCAL
name|PROTO_LOCAL
init|=
literal|1
block|,
DECL|enumerator|PROTO_SSH
name|PROTO_SSH
block|,
DECL|enumerator|PROTO_GIT
name|PROTO_GIT
block|, }
enum|;
end_enum
begin_function
DECL|function|get_protocol
specifier|static
name|enum
name|protocol
name|get_protocol
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|name
argument_list|,
literal|"ssh"
argument_list|)
condition|)
return|return
name|PROTO_SSH
return|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|name
argument_list|,
literal|"git"
argument_list|)
condition|)
return|return
name|PROTO_GIT
return|;
name|die
argument_list|(
literal|"I don't handle protocol '%s'"
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
end_function
begin_define
DECL|macro|STR_
define|#
directive|define
name|STR_
parameter_list|(
name|s
parameter_list|)
value|# s
end_define
begin_define
DECL|macro|STR
define|#
directive|define
name|STR
parameter_list|(
name|s
parameter_list|)
value|STR_(s)
end_define
begin_function
DECL|function|git_tcp_connect
specifier|static
name|int
name|git_tcp_connect
parameter_list|(
name|int
name|fd
index|[
literal|2
index|]
parameter_list|,
specifier|const
name|char
modifier|*
name|prog
parameter_list|,
name|char
modifier|*
name|host
parameter_list|,
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|sockfd
init|=
operator|-
literal|1
decl_stmt|;
name|char
modifier|*
name|colon
decl_stmt|,
modifier|*
name|end
decl_stmt|;
name|char
modifier|*
name|port
init|=
name|STR
argument_list|(
name|DEFAULT_GIT_PORT
argument_list|)
decl_stmt|;
name|struct
name|addrinfo
name|hints
decl_stmt|,
modifier|*
name|ai0
decl_stmt|,
modifier|*
name|ai
decl_stmt|;
name|int
name|gai
decl_stmt|;
if|if
condition|(
name|host
index|[
literal|0
index|]
operator|==
literal|'['
condition|)
block|{
name|end
operator|=
name|strchr
argument_list|(
name|host
operator|+
literal|1
argument_list|,
literal|']'
argument_list|)
expr_stmt|;
if|if
condition|(
name|end
condition|)
block|{
operator|*
name|end
operator|=
literal|0
expr_stmt|;
name|end
operator|++
expr_stmt|;
name|host
operator|++
expr_stmt|;
block|}
else|else
name|end
operator|=
name|host
expr_stmt|;
block|}
else|else
name|end
operator|=
name|host
expr_stmt|;
name|colon
operator|=
name|strchr
argument_list|(
name|end
argument_list|,
literal|':'
argument_list|)
expr_stmt|;
if|if
condition|(
name|colon
condition|)
block|{
operator|*
name|colon
operator|=
literal|0
expr_stmt|;
name|port
operator|=
name|colon
operator|+
literal|1
expr_stmt|;
block|}
name|memset
argument_list|(
operator|&
name|hints
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|hints
argument_list|)
argument_list|)
expr_stmt|;
name|hints
operator|.
name|ai_socktype
operator|=
name|SOCK_STREAM
expr_stmt|;
name|hints
operator|.
name|ai_protocol
operator|=
name|IPPROTO_TCP
expr_stmt|;
name|gai
operator|=
name|getaddrinfo
argument_list|(
name|host
argument_list|,
name|port
argument_list|,
operator|&
name|hints
argument_list|,
operator|&
name|ai
argument_list|)
expr_stmt|;
if|if
condition|(
name|gai
condition|)
name|die
argument_list|(
literal|"Unable to look up %s (%s)"
argument_list|,
name|host
argument_list|,
name|gai_strerror
argument_list|(
name|gai
argument_list|)
argument_list|)
expr_stmt|;
for|for
control|(
name|ai0
operator|=
name|ai
init|;
name|ai
condition|;
name|ai
operator|=
name|ai
operator|->
name|ai_next
control|)
block|{
name|sockfd
operator|=
name|socket
argument_list|(
name|ai
operator|->
name|ai_family
argument_list|,
name|ai
operator|->
name|ai_socktype
argument_list|,
name|ai
operator|->
name|ai_protocol
argument_list|)
expr_stmt|;
if|if
condition|(
name|sockfd
operator|<
literal|0
condition|)
continue|continue;
if|if
condition|(
name|connect
argument_list|(
name|sockfd
argument_list|,
name|ai
operator|->
name|ai_addr
argument_list|,
name|ai
operator|->
name|ai_addrlen
argument_list|)
operator|<
literal|0
condition|)
block|{
name|close
argument_list|(
name|sockfd
argument_list|)
expr_stmt|;
name|sockfd
operator|=
operator|-
literal|1
expr_stmt|;
continue|continue;
block|}
break|break;
block|}
name|freeaddrinfo
argument_list|(
name|ai0
argument_list|)
expr_stmt|;
if|if
condition|(
name|sockfd
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to connect a socket (%s)"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|fd
index|[
literal|0
index|]
operator|=
name|sockfd
expr_stmt|;
name|fd
index|[
literal|1
index|]
operator|=
name|sockfd
expr_stmt|;
name|packet_write
argument_list|(
name|sockfd
argument_list|,
literal|"%s %s\n"
argument_list|,
name|prog
argument_list|,
name|path
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/*  * Yeah, yeah, fixme. Need to pass in the heads etc.  */
end_comment
begin_function
DECL|function|git_connect
name|int
name|git_connect
parameter_list|(
name|int
name|fd
index|[
literal|2
index|]
parameter_list|,
name|char
modifier|*
name|url
parameter_list|,
specifier|const
name|char
modifier|*
name|prog
parameter_list|)
block|{
name|char
name|command
index|[
literal|1024
index|]
decl_stmt|;
name|char
modifier|*
name|host
decl_stmt|,
modifier|*
name|path
decl_stmt|;
name|char
modifier|*
name|colon
decl_stmt|;
name|int
name|pipefd
index|[
literal|2
index|]
index|[
literal|2
index|]
decl_stmt|;
name|pid_t
name|pid
decl_stmt|;
name|enum
name|protocol
name|protocol
decl_stmt|;
name|host
operator|=
name|NULL
expr_stmt|;
name|path
operator|=
name|url
expr_stmt|;
name|colon
operator|=
name|strchr
argument_list|(
name|url
argument_list|,
literal|':'
argument_list|)
expr_stmt|;
name|protocol
operator|=
name|PROTO_LOCAL
expr_stmt|;
if|if
condition|(
name|colon
condition|)
block|{
operator|*
name|colon
operator|=
literal|0
expr_stmt|;
name|host
operator|=
name|url
expr_stmt|;
name|path
operator|=
name|colon
operator|+
literal|1
expr_stmt|;
name|protocol
operator|=
name|PROTO_SSH
expr_stmt|;
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|path
argument_list|,
literal|"//"
argument_list|,
literal|2
argument_list|)
condition|)
block|{
name|char
modifier|*
name|slash
init|=
name|strchr
argument_list|(
name|path
operator|+
literal|2
argument_list|,
literal|'/'
argument_list|)
decl_stmt|;
if|if
condition|(
name|slash
condition|)
block|{
name|int
name|nr
init|=
name|slash
operator|-
name|path
operator|-
literal|2
decl_stmt|;
name|memmove
argument_list|(
name|path
argument_list|,
name|path
operator|+
literal|2
argument_list|,
name|nr
argument_list|)
expr_stmt|;
name|path
index|[
name|nr
index|]
operator|=
literal|0
expr_stmt|;
name|protocol
operator|=
name|get_protocol
argument_list|(
name|url
argument_list|)
expr_stmt|;
name|host
operator|=
name|path
expr_stmt|;
name|path
operator|=
name|slash
expr_stmt|;
block|}
block|}
block|}
if|if
condition|(
name|protocol
operator|==
name|PROTO_GIT
condition|)
return|return
name|git_tcp_connect
argument_list|(
name|fd
argument_list|,
name|prog
argument_list|,
name|host
argument_list|,
name|path
argument_list|)
return|;
if|if
condition|(
name|pipe
argument_list|(
name|pipefd
index|[
literal|0
index|]
argument_list|)
operator|<
literal|0
operator|||
name|pipe
argument_list|(
name|pipefd
index|[
literal|1
index|]
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to create pipe pair for communication"
argument_list|)
expr_stmt|;
name|pid
operator|=
name|fork
argument_list|()
expr_stmt|;
if|if
condition|(
operator|!
name|pid
condition|)
block|{
name|snprintf
argument_list|(
name|command
argument_list|,
sizeof|sizeof
argument_list|(
name|command
argument_list|)
argument_list|,
literal|"%s %s"
argument_list|,
name|prog
argument_list|,
name|sq_quote
argument_list|(
name|path
argument_list|)
argument_list|)
expr_stmt|;
name|dup2
argument_list|(
name|pipefd
index|[
literal|1
index|]
index|[
literal|0
index|]
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|dup2
argument_list|(
name|pipefd
index|[
literal|0
index|]
index|[
literal|1
index|]
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|pipefd
index|[
literal|0
index|]
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|pipefd
index|[
literal|0
index|]
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|pipefd
index|[
literal|1
index|]
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|pipefd
index|[
literal|1
index|]
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|protocol
operator|==
name|PROTO_SSH
condition|)
name|execlp
argument_list|(
literal|"ssh"
argument_list|,
literal|"ssh"
argument_list|,
name|host
argument_list|,
name|command
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
else|else
name|execlp
argument_list|(
literal|"sh"
argument_list|,
literal|"sh"
argument_list|,
literal|"-c"
argument_list|,
name|command
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"exec failed"
argument_list|)
expr_stmt|;
block|}
name|fd
index|[
literal|0
index|]
operator|=
name|pipefd
index|[
literal|0
index|]
index|[
literal|0
index|]
expr_stmt|;
name|fd
index|[
literal|1
index|]
operator|=
name|pipefd
index|[
literal|1
index|]
index|[
literal|1
index|]
expr_stmt|;
name|close
argument_list|(
name|pipefd
index|[
literal|0
index|]
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|pipefd
index|[
literal|1
index|]
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
return|return
name|pid
return|;
block|}
end_function
begin_function
DECL|function|finish_connect
name|int
name|finish_connect
parameter_list|(
name|pid_t
name|pid
parameter_list|)
block|{
name|int
name|ret
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|ret
operator|=
name|waitpid
argument_list|(
name|pid
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
break|break;
if|if
condition|(
name|errno
operator|!=
name|EINTR
condition|)
break|break;
block|}
return|return
name|ret
return|;
block|}
end_function
end_unit
