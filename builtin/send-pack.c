begin_unit
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"pkt-line.h"
end_include
begin_include
include|#
directive|include
file|"sideband.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"remote.h"
end_include
begin_include
include|#
directive|include
file|"connect.h"
end_include
begin_include
include|#
directive|include
file|"send-pack.h"
end_include
begin_include
include|#
directive|include
file|"quote.h"
end_include
begin_include
include|#
directive|include
file|"transport.h"
end_include
begin_include
include|#
directive|include
file|"version.h"
end_include
begin_include
include|#
directive|include
file|"sha1-array.h"
end_include
begin_include
include|#
directive|include
file|"gpg-interface.h"
end_include
begin_include
include|#
directive|include
file|"gettext.h"
end_include
begin_decl_stmt
DECL|variable|send_pack_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|send_pack_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git send-pack [--all | --mirror] [--dry-run] [--force] "
literal|"[--receive-pack=<git-receive-pack>] [--verbose] [--thin] [--atomic] "
literal|"[<host>:]<directory> [<ref>...]\n"
literal|"  --all and explicit<ref> specification are mutually exclusive."
argument_list|)
block|,
name|NULL
block|, }
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|args
specifier|static
name|struct
name|send_pack_args
name|args
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|print_helper_status
specifier|static
name|void
name|print_helper_status
parameter_list|(
name|struct
name|ref
modifier|*
name|ref
parameter_list|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
for|for
control|(
init|;
name|ref
condition|;
name|ref
operator|=
name|ref
operator|->
name|next
control|)
block|{
specifier|const
name|char
modifier|*
name|msg
init|=
name|NULL
decl_stmt|;
specifier|const
name|char
modifier|*
name|res
decl_stmt|;
switch|switch
condition|(
name|ref
operator|->
name|status
condition|)
block|{
case|case
name|REF_STATUS_NONE
case|:
name|res
operator|=
literal|"error"
expr_stmt|;
name|msg
operator|=
literal|"no match"
expr_stmt|;
break|break;
case|case
name|REF_STATUS_OK
case|:
name|res
operator|=
literal|"ok"
expr_stmt|;
break|break;
case|case
name|REF_STATUS_UPTODATE
case|:
name|res
operator|=
literal|"ok"
expr_stmt|;
name|msg
operator|=
literal|"up to date"
expr_stmt|;
break|break;
case|case
name|REF_STATUS_REJECT_NONFASTFORWARD
case|:
name|res
operator|=
literal|"error"
expr_stmt|;
name|msg
operator|=
literal|"non-fast forward"
expr_stmt|;
break|break;
case|case
name|REF_STATUS_REJECT_FETCH_FIRST
case|:
name|res
operator|=
literal|"error"
expr_stmt|;
name|msg
operator|=
literal|"fetch first"
expr_stmt|;
break|break;
case|case
name|REF_STATUS_REJECT_NEEDS_FORCE
case|:
name|res
operator|=
literal|"error"
expr_stmt|;
name|msg
operator|=
literal|"needs force"
expr_stmt|;
break|break;
case|case
name|REF_STATUS_REJECT_STALE
case|:
name|res
operator|=
literal|"error"
expr_stmt|;
name|msg
operator|=
literal|"stale info"
expr_stmt|;
break|break;
case|case
name|REF_STATUS_REJECT_ALREADY_EXISTS
case|:
name|res
operator|=
literal|"error"
expr_stmt|;
name|msg
operator|=
literal|"already exists"
expr_stmt|;
break|break;
case|case
name|REF_STATUS_REJECT_NODELETE
case|:
case|case
name|REF_STATUS_REMOTE_REJECT
case|:
name|res
operator|=
literal|"error"
expr_stmt|;
break|break;
case|case
name|REF_STATUS_EXPECTING_REPORT
case|:
default|default:
continue|continue;
block|}
name|strbuf_reset
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|buf
argument_list|,
literal|"%s %s"
argument_list|,
name|res
argument_list|,
name|ref
operator|->
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
name|ref
operator|->
name|remote_status
condition|)
name|msg
operator|=
name|ref
operator|->
name|remote_status
expr_stmt|;
if|if
condition|(
name|msg
condition|)
block|{
name|strbuf_addch
argument_list|(
operator|&
name|buf
argument_list|,
literal|' '
argument_list|)
expr_stmt|;
name|quote_two_c_style
argument_list|(
operator|&
name|buf
argument_list|,
literal|""
argument_list|,
name|msg
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
name|strbuf_addch
argument_list|(
operator|&
name|buf
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
literal|1
argument_list|,
name|buf
operator|.
name|buf
argument_list|,
name|buf
operator|.
name|len
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|send_pack_config
specifier|static
name|int
name|send_pack_config
parameter_list|(
specifier|const
name|char
modifier|*
name|k
parameter_list|,
specifier|const
name|char
modifier|*
name|v
parameter_list|,
name|void
modifier|*
name|cb
parameter_list|)
block|{
name|git_gpg_config
argument_list|(
name|k
argument_list|,
name|v
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|k
argument_list|,
literal|"push.gpgsign"
argument_list|)
condition|)
block|{
specifier|const
name|char
modifier|*
name|value
decl_stmt|;
if|if
condition|(
operator|!
name|git_config_get_value
argument_list|(
literal|"push.gpgsign"
argument_list|,
operator|&
name|value
argument_list|)
condition|)
block|{
switch|switch
condition|(
name|git_config_maybe_bool
argument_list|(
literal|"push.gpgsign"
argument_list|,
name|value
argument_list|)
condition|)
block|{
case|case
literal|0
case|:
name|args
operator|.
name|push_cert
operator|=
name|SEND_PACK_PUSH_CERT_NEVER
expr_stmt|;
break|break;
case|case
literal|1
case|:
name|args
operator|.
name|push_cert
operator|=
name|SEND_PACK_PUSH_CERT_ALWAYS
expr_stmt|;
break|break;
default|default:
if|if
condition|(
name|value
operator|&&
operator|!
name|strcasecmp
argument_list|(
name|value
argument_list|,
literal|"if-asked"
argument_list|)
condition|)
name|args
operator|.
name|push_cert
operator|=
name|SEND_PACK_PUSH_CERT_IF_ASKED
expr_stmt|;
else|else
return|return
name|error
argument_list|(
literal|"Invalid value for '%s'"
argument_list|,
name|k
argument_list|)
return|;
block|}
block|}
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|cmd_send_pack
name|int
name|cmd_send_pack
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
name|int
name|i
decl_stmt|,
name|nr_refspecs
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|refspecs
init|=
name|NULL
decl_stmt|;
specifier|const
name|char
modifier|*
name|remote_name
init|=
name|NULL
decl_stmt|;
name|struct
name|remote
modifier|*
name|remote
init|=
name|NULL
decl_stmt|;
specifier|const
name|char
modifier|*
name|dest
init|=
name|NULL
decl_stmt|;
name|int
name|fd
index|[
literal|2
index|]
decl_stmt|;
name|struct
name|child_process
modifier|*
name|conn
decl_stmt|;
name|struct
name|sha1_array
name|extra_have
init|=
name|SHA1_ARRAY_INIT
decl_stmt|;
name|struct
name|sha1_array
name|shallow
init|=
name|SHA1_ARRAY_INIT
decl_stmt|;
name|struct
name|ref
modifier|*
name|remote_refs
decl_stmt|,
modifier|*
name|local_refs
decl_stmt|;
name|int
name|ret
decl_stmt|;
name|int
name|helper_status
init|=
literal|0
decl_stmt|;
name|int
name|send_all
init|=
literal|0
decl_stmt|;
name|int
name|verbose
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|receivepack
init|=
literal|"git-receive-pack"
decl_stmt|;
name|unsigned
name|dry_run
init|=
literal|0
decl_stmt|;
name|unsigned
name|send_mirror
init|=
literal|0
decl_stmt|;
name|unsigned
name|force_update
init|=
literal|0
decl_stmt|;
name|unsigned
name|quiet
init|=
literal|0
decl_stmt|;
name|int
name|push_cert
init|=
literal|0
decl_stmt|;
name|unsigned
name|use_thin_pack
init|=
literal|0
decl_stmt|;
name|unsigned
name|atomic
init|=
literal|0
decl_stmt|;
name|unsigned
name|stateless_rpc
init|=
literal|0
decl_stmt|;
name|int
name|flags
decl_stmt|;
name|unsigned
name|int
name|reject_reasons
decl_stmt|;
name|int
name|progress
init|=
operator|-
literal|1
decl_stmt|;
name|int
name|from_stdin
init|=
literal|0
decl_stmt|;
name|struct
name|push_cas_option
name|cas
init|=
block|{
literal|0
block|}
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT__VERBOSITY
argument_list|(
operator|&
name|verbose
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"receive-pack"
argument_list|,
operator|&
name|receivepack
argument_list|,
literal|"receive-pack"
argument_list|,
name|N_
argument_list|(
literal|"receive pack program"
argument_list|)
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"exec"
argument_list|,
operator|&
name|receivepack
argument_list|,
literal|"receive-pack"
argument_list|,
name|N_
argument_list|(
literal|"receive pack program"
argument_list|)
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"remote"
argument_list|,
operator|&
name|remote_name
argument_list|,
literal|"remote"
argument_list|,
name|N_
argument_list|(
literal|"remote name"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"all"
argument_list|,
operator|&
name|send_all
argument_list|,
name|N_
argument_list|(
literal|"push all refs"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|'n'
argument_list|,
literal|"dry-run"
argument_list|,
operator|&
name|dry_run
argument_list|,
name|N_
argument_list|(
literal|"dry run"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"mirror"
argument_list|,
operator|&
name|send_mirror
argument_list|,
name|N_
argument_list|(
literal|"mirror all refs"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|'f'
argument_list|,
literal|"force"
argument_list|,
operator|&
name|force_update
argument_list|,
name|N_
argument_list|(
literal|"force updates"
argument_list|)
argument_list|)
block|,
block|{
name|OPTION_CALLBACK
block|,
literal|0
block|,
literal|"signed"
block|,
operator|&
name|push_cert
block|,
literal|"yes|no|if-asked"
block|,
name|N_
argument_list|(
literal|"GPG sign the push"
argument_list|)
block|,
name|PARSE_OPT_OPTARG
block|,
name|option_parse_push_signed
block|}
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"progress"
argument_list|,
operator|&
name|progress
argument_list|,
name|N_
argument_list|(
literal|"force progress reporting"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"thin"
argument_list|,
operator|&
name|use_thin_pack
argument_list|,
name|N_
argument_list|(
literal|"use thin pack"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"atomic"
argument_list|,
operator|&
name|atomic
argument_list|,
name|N_
argument_list|(
literal|"request atomic transaction on remote side"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"stateless-rpc"
argument_list|,
operator|&
name|stateless_rpc
argument_list|,
name|N_
argument_list|(
literal|"use stateless RPC protocol"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"stdin"
argument_list|,
operator|&
name|from_stdin
argument_list|,
name|N_
argument_list|(
literal|"read refs from stdin"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"helper-status"
argument_list|,
operator|&
name|helper_status
argument_list|,
name|N_
argument_list|(
literal|"print status from remote helper"
argument_list|)
argument_list|)
block|,
block|{
name|OPTION_CALLBACK
block|,
literal|0
block|,
name|CAS_OPT_NAME
block|,
operator|&
name|cas
block|,
name|N_
argument_list|(
literal|"refname>:<expect"
argument_list|)
block|,
name|N_
argument_list|(
literal|"require old value of ref to be at this value"
argument_list|)
block|,
name|PARSE_OPT_OPTARG
block|,
name|parseopt_push_cas_option
block|}
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
name|git_config
argument_list|(
name|send_pack_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
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
name|send_pack_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|>
literal|0
condition|)
block|{
name|dest
operator|=
name|argv
index|[
literal|0
index|]
expr_stmt|;
name|refspecs
operator|=
operator|(
specifier|const
name|char
operator|*
operator|*
operator|)
operator|(
name|argv
operator|+
literal|1
operator|)
expr_stmt|;
name|nr_refspecs
operator|=
name|argc
operator|-
literal|1
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|dest
condition|)
name|usage_with_options
argument_list|(
name|send_pack_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
name|args
operator|.
name|verbose
operator|=
name|verbose
expr_stmt|;
name|args
operator|.
name|dry_run
operator|=
name|dry_run
expr_stmt|;
name|args
operator|.
name|send_mirror
operator|=
name|send_mirror
expr_stmt|;
name|args
operator|.
name|force_update
operator|=
name|force_update
expr_stmt|;
name|args
operator|.
name|quiet
operator|=
name|quiet
expr_stmt|;
name|args
operator|.
name|push_cert
operator|=
name|push_cert
expr_stmt|;
name|args
operator|.
name|progress
operator|=
name|progress
expr_stmt|;
name|args
operator|.
name|use_thin_pack
operator|=
name|use_thin_pack
expr_stmt|;
name|args
operator|.
name|atomic
operator|=
name|atomic
expr_stmt|;
name|args
operator|.
name|stateless_rpc
operator|=
name|stateless_rpc
expr_stmt|;
if|if
condition|(
name|from_stdin
condition|)
block|{
name|struct
name|argv_array
name|all_refspecs
init|=
name|ARGV_ARRAY_INIT
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_refspecs
condition|;
name|i
operator|++
control|)
name|argv_array_push
argument_list|(
operator|&
name|all_refspecs
argument_list|,
name|refspecs
index|[
name|i
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|args
operator|.
name|stateless_rpc
condition|)
block|{
specifier|const
name|char
modifier|*
name|buf
decl_stmt|;
while|while
condition|(
operator|(
name|buf
operator|=
name|packet_read_line
argument_list|(
literal|0
argument_list|,
name|NULL
argument_list|)
operator|)
condition|)
name|argv_array_push
argument_list|(
operator|&
name|all_refspecs
argument_list|,
name|buf
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|struct
name|strbuf
name|line
init|=
name|STRBUF_INIT
decl_stmt|;
while|while
condition|(
name|strbuf_getline_lf
argument_list|(
operator|&
name|line
argument_list|,
name|stdin
argument_list|)
operator|!=
name|EOF
condition|)
name|argv_array_push
argument_list|(
operator|&
name|all_refspecs
argument_list|,
name|line
operator|.
name|buf
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|line
argument_list|)
expr_stmt|;
block|}
name|refspecs
operator|=
name|all_refspecs
operator|.
name|argv
expr_stmt|;
name|nr_refspecs
operator|=
name|all_refspecs
operator|.
name|argc
expr_stmt|;
block|}
comment|/* 	 * --all and --mirror are incompatible; neither makes sense 	 * with any refspecs. 	 */
if|if
condition|(
operator|(
name|refspecs
operator|&&
operator|(
name|send_all
operator|||
name|args
operator|.
name|send_mirror
operator|)
operator|)
operator|||
operator|(
name|send_all
operator|&&
name|args
operator|.
name|send_mirror
operator|)
condition|)
name|usage_with_options
argument_list|(
name|send_pack_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
if|if
condition|(
name|remote_name
condition|)
block|{
name|remote
operator|=
name|remote_get
argument_list|(
name|remote_name
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|remote_has_url
argument_list|(
name|remote
argument_list|,
name|dest
argument_list|)
condition|)
block|{
name|die
argument_list|(
literal|"Destination %s is not a uri for %s"
argument_list|,
name|dest
argument_list|,
name|remote_name
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|progress
operator|==
operator|-
literal|1
condition|)
name|progress
operator|=
operator|!
name|args
operator|.
name|quiet
operator|&&
name|isatty
argument_list|(
literal|2
argument_list|)
expr_stmt|;
name|args
operator|.
name|progress
operator|=
name|progress
expr_stmt|;
if|if
condition|(
name|args
operator|.
name|stateless_rpc
condition|)
block|{
name|conn
operator|=
name|NULL
expr_stmt|;
name|fd
index|[
literal|0
index|]
operator|=
literal|0
expr_stmt|;
name|fd
index|[
literal|1
index|]
operator|=
literal|1
expr_stmt|;
block|}
else|else
block|{
name|conn
operator|=
name|git_connect
argument_list|(
name|fd
argument_list|,
name|dest
argument_list|,
name|receivepack
argument_list|,
name|args
operator|.
name|verbose
condition|?
name|CONNECT_VERBOSE
else|:
literal|0
argument_list|)
expr_stmt|;
block|}
name|get_remote_heads
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
operator|&
name|remote_refs
argument_list|,
name|REF_NORMAL
argument_list|,
operator|&
name|extra_have
argument_list|,
operator|&
name|shallow
argument_list|)
expr_stmt|;
name|transport_verify_remote_names
argument_list|(
name|nr_refspecs
argument_list|,
name|refspecs
argument_list|)
expr_stmt|;
name|local_refs
operator|=
name|get_local_heads
argument_list|()
expr_stmt|;
name|flags
operator|=
name|MATCH_REFS_NONE
expr_stmt|;
if|if
condition|(
name|send_all
condition|)
name|flags
operator||=
name|MATCH_REFS_ALL
expr_stmt|;
if|if
condition|(
name|args
operator|.
name|send_mirror
condition|)
name|flags
operator||=
name|MATCH_REFS_MIRROR
expr_stmt|;
comment|/* match them up */
if|if
condition|(
name|match_push_refs
argument_list|(
name|local_refs
argument_list|,
operator|&
name|remote_refs
argument_list|,
name|nr_refspecs
argument_list|,
name|refspecs
argument_list|,
name|flags
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
operator|!
name|is_empty_cas
argument_list|(
operator|&
name|cas
argument_list|)
condition|)
name|apply_push_cas
argument_list|(
operator|&
name|cas
argument_list|,
name|remote
argument_list|,
name|remote_refs
argument_list|)
expr_stmt|;
name|set_ref_status_for_push
argument_list|(
name|remote_refs
argument_list|,
name|args
operator|.
name|send_mirror
argument_list|,
name|args
operator|.
name|force_update
argument_list|)
expr_stmt|;
name|ret
operator|=
name|send_pack
argument_list|(
operator|&
name|args
argument_list|,
name|fd
argument_list|,
name|conn
argument_list|,
name|remote_refs
argument_list|,
operator|&
name|extra_have
argument_list|)
expr_stmt|;
if|if
condition|(
name|helper_status
condition|)
name|print_helper_status
argument_list|(
name|remote_refs
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|ret
operator||=
name|finish_connect
argument_list|(
name|conn
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|helper_status
condition|)
name|transport_print_push_status
argument_list|(
name|dest
argument_list|,
name|remote_refs
argument_list|,
name|args
operator|.
name|verbose
argument_list|,
literal|0
argument_list|,
operator|&
name|reject_reasons
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|args
operator|.
name|dry_run
operator|&&
name|remote
condition|)
block|{
name|struct
name|ref
modifier|*
name|ref
decl_stmt|;
for|for
control|(
name|ref
operator|=
name|remote_refs
init|;
name|ref
condition|;
name|ref
operator|=
name|ref
operator|->
name|next
control|)
name|transport_update_tracking_ref
argument_list|(
name|remote
argument_list|,
name|ref
argument_list|,
name|args
operator|.
name|verbose
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|ret
operator|&&
operator|!
name|transport_refs_pushed
argument_list|(
name|remote_refs
argument_list|)
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Everything up-to-date\n"
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
end_unit
