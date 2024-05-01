# Linked-List
Submodule utility for my other repositories.

Expect the caller of this linked list is only one and not referenced from multiple entities (such as process or threads).

## Notes
API incompatibility occurs between two commit ids `4acc2b0` and `58225f0`. The APIs that returned `node *` before have been updated to return `void *` directly, representing pointer to application data.
