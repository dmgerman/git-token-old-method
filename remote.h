begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|REMOTE_H
end_ifndef
begin_define
DECL|macro|REMOTE_H
define|#
directive|define
name|REMOTE_H
end_define
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"hashmap.h"
end_include
begin_enum
enum|enum
block|{
DECL|enumerator|REMOTE_UNCONFIGURED
name|REMOTE_UNCONFIGURED
init|=
literal|0
block|,
DECL|enumerator|REMOTE_CONFIG
name|REMOTE_CONFIG
block|,
DECL|enumerator|REMOTE_REMOTES
name|REMOTE_REMOTES
block|,
DECL|enumerator|REMOTE_BRANCHES
name|REMOTE_BRANCHES
block|}
enum|;
end_enum
begin_struct
DECL|struct|remote
struct|struct
name|remote
block|{
DECL|member|ent
name|struct
name|hashmap_entry
name|ent
decl_stmt|;
comment|/* must be first */
DECL|member|name
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|origin
name|int
name|origin
decl_stmt|;
DECL|member|foreign_vcs
specifier|const
name|char
modifier|*
name|foreign_vcs
decl_stmt|;
DECL|member|url
specifier|const
name|char
modifier|*
modifier|*
name|url
decl_stmt|;
DECL|member|url_nr
name|int
name|url_nr
decl_stmt|;
DECL|member|url_alloc
name|int
name|url_alloc
decl_stmt|;
DECL|member|pushurl
specifier|const
name|char
modifier|*
modifier|*
name|pushurl
decl_stmt|;
DECL|member|pushurl_nr
name|int
name|pushurl_nr
decl_stmt|;
DECL|member|pushurl_alloc
name|int
name|pushurl_alloc
decl_stmt|;
DECL|member|push_refspec
specifier|const
name|char
modifier|*
modifier|*
name|push_refspec
decl_stmt|;
DECL|member|push
name|struct
name|refspec
modifier|*
name|push
decl_stmt|;
DECL|member|push_refspec_nr
name|int
name|push_refspec_nr
decl_stmt|;
DECL|member|push_refspec_alloc
name|int
name|push_refspec_alloc
decl_stmt|;
DECL|member|fetch_refspec
specifier|const
name|char
modifier|*
modifier|*
name|fetch_refspec
decl_stmt|;
DECL|member|fetch
name|struct
name|refspec
modifier|*
name|fetch
decl_stmt|;
DECL|member|fetch_refspec_nr
name|int
name|fetch_refspec_nr
decl_stmt|;
DECL|member|fetch_refspec_alloc
name|int
name|fetch_refspec_alloc
decl_stmt|;
comment|/* 	 * -1 to never fetch tags 	 * 0 to auto-follow tags on heuristic (default) 	 * 1 to always auto-follow tags 	 * 2 to always fetch tags 	 */
DECL|member|fetch_tags
name|int
name|fetch_tags
decl_stmt|;
DECL|member|skip_default_update
name|int
name|skip_default_update
decl_stmt|;
DECL|member|mirror
name|int
name|mirror
decl_stmt|;
DECL|member|prune
name|int
name|prune
decl_stmt|;
DECL|member|receivepack
specifier|const
name|char
modifier|*
name|receivepack
decl_stmt|;
DECL|member|uploadpack
specifier|const
name|char
modifier|*
name|uploadpack
decl_stmt|;
comment|/* 	 * for curl remotes only 	 */
DECL|member|http_proxy
name|char
modifier|*
name|http_proxy
decl_stmt|;
DECL|member|http_proxy_authmethod
name|char
modifier|*
name|http_proxy_authmethod
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
name|struct
name|remote
modifier|*
name|remote_get
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|remote
modifier|*
name|pushremote_get
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|remote_is_configured
parameter_list|(
name|struct
name|remote
modifier|*
name|remote
parameter_list|)
function_decl|;
end_function_decl
begin_typedef
DECL|typedef|each_remote_fn
typedef|typedef
name|int
name|each_remote_fn
parameter_list|(
name|struct
name|remote
modifier|*
name|remote
parameter_list|,
name|void
modifier|*
name|priv
parameter_list|)
function_decl|;
end_typedef
begin_function_decl
name|int
name|for_each_remote
parameter_list|(
name|each_remote_fn
name|fn
parameter_list|,
name|void
modifier|*
name|priv
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|remote_has_url
parameter_list|(
name|struct
name|remote
modifier|*
name|remote
parameter_list|,
specifier|const
name|char
modifier|*
name|url
parameter_list|)
function_decl|;
end_function_decl
begin_struct
DECL|struct|refspec
struct|struct
name|refspec
block|{
DECL|member|force
name|unsigned
name|force
range|:
literal|1
decl_stmt|;
DECL|member|pattern
name|unsigned
name|pattern
range|:
literal|1
decl_stmt|;
DECL|member|matching
name|unsigned
name|matching
range|:
literal|1
decl_stmt|;
DECL|member|exact_sha1
name|unsigned
name|exact_sha1
range|:
literal|1
decl_stmt|;
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
begin_decl_stmt
specifier|extern
specifier|const
name|struct
name|refspec
modifier|*
name|tag_refspec
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|ref
struct|struct
name|ref
block|{
DECL|member|next
name|struct
name|ref
modifier|*
name|next
decl_stmt|;
DECL|member|old_oid
name|struct
name|object_id
name|old_oid
decl_stmt|;
DECL|member|new_oid
name|struct
name|object_id
name|new_oid
decl_stmt|;
DECL|member|old_oid_expect
name|struct
name|object_id
name|old_oid_expect
decl_stmt|;
comment|/* used by expect-old */
DECL|member|symref
name|char
modifier|*
name|symref
decl_stmt|;
name|unsigned
name|int
DECL|member|force
name|force
range|:
literal|1
decl_stmt|,
DECL|member|forced_update
name|forced_update
range|:
literal|1
decl_stmt|,
DECL|member|expect_old_sha1
name|expect_old_sha1
range|:
literal|1
decl_stmt|,
DECL|member|expect_old_no_trackback
name|expect_old_no_trackback
range|:
literal|1
decl_stmt|,
DECL|member|deletion
name|deletion
range|:
literal|1
decl_stmt|,
DECL|member|matched
name|matched
range|:
literal|1
decl_stmt|;
comment|/* 	 * Order is important here, as we write to FETCH_HEAD 	 * in numeric order. And the default NOT_FOR_MERGE 	 * should be 0, so that xcalloc'd structures get it 	 * by default. 	 */
enum|enum
block|{
DECL|enumerator|FETCH_HEAD_MERGE
name|FETCH_HEAD_MERGE
init|=
operator|-
literal|1
block|,
DECL|enumerator|FETCH_HEAD_NOT_FOR_MERGE
name|FETCH_HEAD_NOT_FOR_MERGE
init|=
literal|0
block|,
DECL|enumerator|FETCH_HEAD_IGNORE
name|FETCH_HEAD_IGNORE
init|=
literal|1
block|}
DECL|member|fetch_head_status
name|fetch_head_status
enum|;
enum|enum
block|{
DECL|enumerator|REF_STATUS_NONE
name|REF_STATUS_NONE
init|=
literal|0
block|,
DECL|enumerator|REF_STATUS_OK
name|REF_STATUS_OK
block|,
DECL|enumerator|REF_STATUS_REJECT_NONFASTFORWARD
name|REF_STATUS_REJECT_NONFASTFORWARD
block|,
DECL|enumerator|REF_STATUS_REJECT_ALREADY_EXISTS
name|REF_STATUS_REJECT_ALREADY_EXISTS
block|,
DECL|enumerator|REF_STATUS_REJECT_NODELETE
name|REF_STATUS_REJECT_NODELETE
block|,
DECL|enumerator|REF_STATUS_REJECT_FETCH_FIRST
name|REF_STATUS_REJECT_FETCH_FIRST
block|,
DECL|enumerator|REF_STATUS_REJECT_NEEDS_FORCE
name|REF_STATUS_REJECT_NEEDS_FORCE
block|,
DECL|enumerator|REF_STATUS_REJECT_STALE
name|REF_STATUS_REJECT_STALE
block|,
DECL|enumerator|REF_STATUS_REJECT_SHALLOW
name|REF_STATUS_REJECT_SHALLOW
block|,
DECL|enumerator|REF_STATUS_UPTODATE
name|REF_STATUS_UPTODATE
block|,
DECL|enumerator|REF_STATUS_REMOTE_REJECT
name|REF_STATUS_REMOTE_REJECT
block|,
DECL|enumerator|REF_STATUS_EXPECTING_REPORT
name|REF_STATUS_EXPECTING_REPORT
block|,
DECL|enumerator|REF_STATUS_ATOMIC_PUSH_FAILED
name|REF_STATUS_ATOMIC_PUSH_FAILED
block|}
DECL|member|status
name|status
enum|;
DECL|member|remote_status
name|char
modifier|*
name|remote_status
decl_stmt|;
DECL|member|peer_ref
name|struct
name|ref
modifier|*
name|peer_ref
decl_stmt|;
comment|/* when renaming */
DECL|member|name
name|char
name|name
index|[
name|FLEX_ARRAY
index|]
decl_stmt|;
comment|/* more */
block|}
struct|;
end_struct
begin_define
DECL|macro|REF_NORMAL
define|#
directive|define
name|REF_NORMAL
value|(1u<< 0)
end_define
begin_define
DECL|macro|REF_HEADS
define|#
directive|define
name|REF_HEADS
value|(1u<< 1)
end_define
begin_define
DECL|macro|REF_TAGS
define|#
directive|define
name|REF_TAGS
value|(1u<< 2)
end_define
begin_function_decl
specifier|extern
name|struct
name|ref
modifier|*
name|find_ref_by_name
parameter_list|(
specifier|const
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
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|ref
modifier|*
name|alloc_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|ref
modifier|*
name|copy_ref
parameter_list|(
specifier|const
name|struct
name|ref
modifier|*
name|ref
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|ref
modifier|*
name|copy_ref_list
parameter_list|(
specifier|const
name|struct
name|ref
modifier|*
name|ref
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|sort_ref_list
parameter_list|(
name|struct
name|ref
modifier|*
modifier|*
parameter_list|,
name|int
function_decl|(
modifier|*
name|cmp
function_decl|)
parameter_list|(
specifier|const
name|void
modifier|*
parameter_list|,
specifier|const
name|void
modifier|*
parameter_list|)
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|count_refspec_match
parameter_list|(
specifier|const
name|char
modifier|*
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
function_decl|;
end_function_decl
begin_function_decl
name|int
name|ref_compare_name
parameter_list|(
specifier|const
name|void
modifier|*
parameter_list|,
specifier|const
name|void
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|check_ref_type
parameter_list|(
specifier|const
name|struct
name|ref
modifier|*
name|ref
parameter_list|,
name|int
name|flags
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * Frees the entire list and peers of elements.  */
end_comment
begin_function_decl
name|void
name|free_refs
parameter_list|(
name|struct
name|ref
modifier|*
name|ref
parameter_list|)
function_decl|;
end_function_decl
begin_struct_decl
struct_decl|struct
name|sha1_array
struct_decl|;
end_struct_decl
begin_function_decl
specifier|extern
name|struct
name|ref
modifier|*
modifier|*
name|get_remote_heads
parameter_list|(
name|int
name|in
parameter_list|,
name|char
modifier|*
name|src_buf
parameter_list|,
name|size_t
name|src_len
parameter_list|,
name|struct
name|ref
modifier|*
modifier|*
name|list
parameter_list|,
name|unsigned
name|int
name|flags
parameter_list|,
name|struct
name|sha1_array
modifier|*
name|extra_have
parameter_list|,
name|struct
name|sha1_array
modifier|*
name|shallow
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|resolve_remote_symref
parameter_list|(
name|struct
name|ref
modifier|*
name|ref
parameter_list|,
name|struct
name|ref
modifier|*
name|list
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|ref_newer
parameter_list|(
specifier|const
name|struct
name|object_id
modifier|*
name|new_oid
parameter_list|,
specifier|const
name|struct
name|object_id
modifier|*
name|old_oid
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * Remove and free all but the first of any entries in the input list  * that map the same remote reference to the same local reference.  If  * there are two entries that map different remote references to the  * same local reference, emit an error message and die.  Return a  * pointer to the head of the resulting list.  */
end_comment
begin_function_decl
name|struct
name|ref
modifier|*
name|ref_remove_duplicates
parameter_list|(
name|struct
name|ref
modifier|*
name|ref_map
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|valid_fetch_refspec
parameter_list|(
specifier|const
name|char
modifier|*
name|refspec
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|refspec
modifier|*
name|parse_fetch_refspec
parameter_list|(
name|int
name|nr_refspec
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|refspec
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|free_refspec
parameter_list|(
name|int
name|nr_refspec
parameter_list|,
name|struct
name|refspec
modifier|*
name|refspec
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|query_refspecs
parameter_list|(
name|struct
name|refspec
modifier|*
name|specs
parameter_list|,
name|int
name|nr
parameter_list|,
name|struct
name|refspec
modifier|*
name|query
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|char
modifier|*
name|apply_refspecs
parameter_list|(
name|struct
name|refspec
modifier|*
name|refspecs
parameter_list|,
name|int
name|nr_refspec
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|check_push_refs
parameter_list|(
name|struct
name|ref
modifier|*
name|src
parameter_list|,
name|int
name|nr_refspec
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|refspec
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|match_push_refs
parameter_list|(
name|struct
name|ref
modifier|*
name|src
parameter_list|,
name|struct
name|ref
modifier|*
modifier|*
name|dst
parameter_list|,
name|int
name|nr_refspec
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|refspec
parameter_list|,
name|int
name|all
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|set_ref_status_for_push
parameter_list|(
name|struct
name|ref
modifier|*
name|remote_refs
parameter_list|,
name|int
name|send_mirror
parameter_list|,
name|int
name|force_update
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * Given a list of the remote refs and the specification of things to  * fetch, makes a (separate) list of the refs to fetch and the local  * refs to store into.  *  * *tail is the pointer to the tail pointer of the list of results  * beforehand, and will be set to the tail pointer of the list of  * results afterward.  *  * missing_ok is usually false, but when we are adding branch.$name.merge  * it is Ok if the branch is not at the remote anymore.  */
end_comment
begin_function_decl
name|int
name|get_fetch_map
parameter_list|(
specifier|const
name|struct
name|ref
modifier|*
name|remote_refs
parameter_list|,
specifier|const
name|struct
name|refspec
modifier|*
name|refspec
parameter_list|,
name|struct
name|ref
modifier|*
modifier|*
modifier|*
name|tail
parameter_list|,
name|int
name|missing_ok
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|ref
modifier|*
name|get_remote_ref
parameter_list|(
specifier|const
name|struct
name|ref
modifier|*
name|remote_refs
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * For the given remote, reads the refspec's src and sets the other fields.  */
end_comment
begin_function_decl
name|int
name|remote_find_tracking
parameter_list|(
name|struct
name|remote
modifier|*
name|remote
parameter_list|,
name|struct
name|refspec
modifier|*
name|refspec
parameter_list|)
function_decl|;
end_function_decl
begin_struct
DECL|struct|branch
struct|struct
name|branch
block|{
DECL|member|name
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|refname
specifier|const
name|char
modifier|*
name|refname
decl_stmt|;
DECL|member|remote_name
specifier|const
name|char
modifier|*
name|remote_name
decl_stmt|;
DECL|member|pushremote_name
specifier|const
name|char
modifier|*
name|pushremote_name
decl_stmt|;
DECL|member|merge_name
specifier|const
name|char
modifier|*
modifier|*
name|merge_name
decl_stmt|;
DECL|member|merge
name|struct
name|refspec
modifier|*
modifier|*
name|merge
decl_stmt|;
DECL|member|merge_nr
name|int
name|merge_nr
decl_stmt|;
DECL|member|merge_alloc
name|int
name|merge_alloc
decl_stmt|;
DECL|member|push_tracking_ref
specifier|const
name|char
modifier|*
name|push_tracking_ref
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
name|struct
name|branch
modifier|*
name|branch_get
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|const
name|char
modifier|*
name|remote_for_branch
parameter_list|(
name|struct
name|branch
modifier|*
name|branch
parameter_list|,
name|int
modifier|*
name|explicit
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|const
name|char
modifier|*
name|pushremote_for_branch
parameter_list|(
name|struct
name|branch
modifier|*
name|branch
parameter_list|,
name|int
modifier|*
name|explicit
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|branch_has_merge_config
parameter_list|(
name|struct
name|branch
modifier|*
name|branch
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|branch_merge_matches
parameter_list|(
name|struct
name|branch
modifier|*
parameter_list|,
name|int
name|n
parameter_list|,
specifier|const
name|char
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/**  * Return the fully-qualified refname of the tracking branch for `branch`.  * I.e., what "branch@{upstream}" would give you. Returns NULL if no  * upstream is defined.  *  * If `err` is not NULL and no upstream is defined, a more specific error  * message is recorded there (if the function does not return NULL, then  * `err` is not touched).  */
end_comment
begin_function_decl
specifier|const
name|char
modifier|*
name|branch_get_upstream
parameter_list|(
name|struct
name|branch
modifier|*
name|branch
parameter_list|,
name|struct
name|strbuf
modifier|*
name|err
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/**  * Return the tracking branch that corresponds to the ref we would push to  * given a bare `git push` while `branch` is checked out.  *  * The return value and `err` conventions match those of `branch_get_upstream`.  */
end_comment
begin_function_decl
specifier|const
name|char
modifier|*
name|branch_get_push
parameter_list|(
name|struct
name|branch
modifier|*
name|branch
parameter_list|,
name|struct
name|strbuf
modifier|*
name|err
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* Flags to match_refs. */
end_comment
begin_enum
DECL|enum|match_refs_flags
enum|enum
name|match_refs_flags
block|{
DECL|enumerator|MATCH_REFS_NONE
name|MATCH_REFS_NONE
init|=
literal|0
block|,
DECL|enumerator|MATCH_REFS_ALL
name|MATCH_REFS_ALL
init|=
operator|(
literal|1
operator|<<
literal|0
operator|)
block|,
DECL|enumerator|MATCH_REFS_MIRROR
name|MATCH_REFS_MIRROR
init|=
operator|(
literal|1
operator|<<
literal|1
operator|)
block|,
DECL|enumerator|MATCH_REFS_PRUNE
name|MATCH_REFS_PRUNE
init|=
operator|(
literal|1
operator|<<
literal|2
operator|)
block|,
DECL|enumerator|MATCH_REFS_FOLLOW_TAGS
name|MATCH_REFS_FOLLOW_TAGS
init|=
operator|(
literal|1
operator|<<
literal|3
operator|)
block|}
enum|;
end_enum
begin_comment
comment|/* Reporting of tracking info */
end_comment
begin_function_decl
name|int
name|stat_tracking_info
parameter_list|(
name|struct
name|branch
modifier|*
name|branch
parameter_list|,
name|int
modifier|*
name|num_ours
parameter_list|,
name|int
modifier|*
name|num_theirs
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|upstream_name
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|format_tracking_info
parameter_list|(
name|struct
name|branch
modifier|*
name|branch
parameter_list|,
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|ref
modifier|*
name|get_local_heads
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * Find refs from a list which are likely to be pointed to by the given HEAD  * ref. If 'all' is false, returns the most likely ref; otherwise, returns a  * list of all candidate refs. If no match is found (or 'head' is NULL),  * returns NULL. All returns are newly allocated and should be freed.  */
end_comment
begin_function_decl
name|struct
name|ref
modifier|*
name|guess_remote_head
parameter_list|(
specifier|const
name|struct
name|ref
modifier|*
name|head
parameter_list|,
specifier|const
name|struct
name|ref
modifier|*
name|refs
parameter_list|,
name|int
name|all
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* Return refs which no longer exist on remote */
end_comment
begin_function_decl
name|struct
name|ref
modifier|*
name|get_stale_heads
parameter_list|(
name|struct
name|refspec
modifier|*
name|refs
parameter_list|,
name|int
name|ref_count
parameter_list|,
name|struct
name|ref
modifier|*
name|fetch_map
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * Compare-and-swap  */
end_comment
begin_define
DECL|macro|CAS_OPT_NAME
define|#
directive|define
name|CAS_OPT_NAME
value|"force-with-lease"
end_define
begin_struct
DECL|struct|push_cas_option
struct|struct
name|push_cas_option
block|{
DECL|member|use_tracking_for_rest
name|unsigned
name|use_tracking_for_rest
range|:
literal|1
decl_stmt|;
DECL|struct|push_cas
struct|struct
name|push_cas
block|{
DECL|member|expect
name|unsigned
name|char
name|expect
index|[
literal|20
index|]
decl_stmt|;
DECL|member|use_tracking
name|unsigned
name|use_tracking
range|:
literal|1
decl_stmt|;
DECL|member|refname
name|char
modifier|*
name|refname
decl_stmt|;
block|}
DECL|member|entry
modifier|*
name|entry
struct|;
DECL|member|nr
name|int
name|nr
decl_stmt|;
DECL|member|alloc
name|int
name|alloc
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
specifier|extern
name|int
name|parseopt_push_cas_option
parameter_list|(
specifier|const
name|struct
name|option
modifier|*
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|parse_push_cas_option
parameter_list|(
name|struct
name|push_cas_option
modifier|*
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|is_empty_cas
parameter_list|(
specifier|const
name|struct
name|push_cas_option
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|apply_push_cas
parameter_list|(
name|struct
name|push_cas_option
modifier|*
parameter_list|,
name|struct
name|remote
modifier|*
parameter_list|,
name|struct
name|ref
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
end_unit
