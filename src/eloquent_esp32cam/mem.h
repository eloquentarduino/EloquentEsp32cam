#pragma once

namespace eloq {
    /**
     * Init buffer to zero
     * @tparam T
     * @param buffer
     * @param size
     * @return
     */
    template<typename T>
    T* zero(T* buffer, size_t size) {
        memset(buffer, 0, size);

        return buffer;
    }
    /**
     * Alias for malloc/ps_malloc
     * @tparam T
     * @param size
     * @return
     */
    template<typename T>
    T* alloc(size_t size) {
        if (psramFound()) {
            return zero(static_cast<T *>(ps_malloc(size)), size);
        }

        return zero(static_cast<T*>(malloc(size)), size);
    }

    /**
     * Alias for realloc/ps_realloc
     * @tparam T
     * @param existing
     * @param size
     * @return
     */
    template<typename T>
    T* realloc(T* existing, size_t size) {
        if (existing == NULL)
            return alloc<T>(size);

        if (psramFound())
            return zero(static_cast<T*>(ps_realloc(existing, size)), size);

        return zero(static_cast<T*>(realloc(existing, size)), size);
    }
}