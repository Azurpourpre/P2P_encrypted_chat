#ifndef _MULTITHREADING
#define _MULTITHREADING

#include <mutex>

class Shared_buffer{
    public:
        Shared_buffer(size_t default_size);
        ~Shared_buffer();

        template<typename T>
        void write(T data);
        
        template<typename T>
        T read();

        size_t current_size;
    private:
        void* buffer;
        std::mutex mutex;
};

Shared_buffer::Shared_buffer(size_t default_size = 4){
    this->buffer = malloc(default_size);
}

Shared_buffer::~Shared_buffer(){
    free(this->buffer);
}

template<typename T>
void Shared_buffer::write(T data){
    this->mutex.lock();

    if(sizeof(T) > current_size){
        this->buffer = realloc(this->buffer, sizeof(T));
        if(this->buffer == NULL)
            err_exit(MEMORY_ERROR);
    }
    memcpy(buffer, &data, sizeof(T));

    this->mutex.unlock();
}

template<typename T>
T Shared_buffer::read(){
    this->mutex.lock();
    T retval = *buffer;
    this->mutex.unlock();
}


#endif