#ifndef ENTLIB_SESSION_H
#define ENTLIB_SESSION_H

struct ent_session *
ent_session_alloc (
    struct ent_processor * p);

void
ent_session_free (
    struct ent_session * s);

int
ent_session_lock (
    struct ent_session * s);

#endif//ENTLIB_SESSION_H
