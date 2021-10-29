#pragma once
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void mspace_get_stats(mspace msp, size_t* out_maxfp, size_t* out_fp, size_t* out_used) {
  mstate m = (mstate)msp;
  if (!ok_magic(m)) {
    USAGE_ERROR_ACTION(m, m);
  }

  ensure_initialization();
  if (!PREACTION(m)) {
    size_t maxfp = 0;
    size_t fp = 0;
    size_t used = 0;
    check_malloc_state(m);
    if (is_initialized(m)) {
      msegmentptr s = &m->seg;
      maxfp = m->max_footprint;
      fp = m->footprint;
      used = fp - (m->topsize + TOP_FOOT_SIZE);

      while (s != 0) {
        mchunkptr q = align_as_chunk(s->base);
        while (segment_holds(s, q) && q != m->top && q->head != FENCEPOST_HEAD) {
          if (!is_inuse(q)) used -= chunksize(q);
          q = next_chunk(q);
        }
        s = s->next;
      }
    }
    POSTACTION(m); /* drop lock */
    *out_maxfp = maxfp;
    *out_fp = fp;
    *out_used = used;
  }
}

mspace base_to_mspace(void* base) {
  mchunkptr msp = align_as_chunk((char*)base);
  mstate m = (mstate)(chunk2mem(msp));
  return (mspace)m;
}

#ifdef __cplusplus
} /* end of extern "C" */
#endif /* __cplusplus */
