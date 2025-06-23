#ifndef VFS_H
#define VFS_H

void init_vfs();
void destroy_vfs();
void vfs_create(const char *name);
void vfs_list();

#endif