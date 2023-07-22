
typedef enum ASCII_Control_Characters {
    BACKSPACE = 127,
    ESC = 27,
    FORM_FEED = 12,
} ASCII_Control_Characters;

typedef struct Buffer {
    unsigned buffer_size;
    unsigned grouth_by;
    unsigned index;
    char buffer[];
} Buffer;

Buffer *create_buffer(unsigned buffer_size, unsigned grouth_by)
{
    Buffer *new_buffer = (Buffer *) malloc(sizeof(Buffer) + sizeof(char) * buffer_size);
    
    new_buffer->buffer_size = buffer_size;
    new_buffer->grouth_by = grouth_by;
    new_buffer->index = 0;

    return new_buffer;
}

bool buffer_ensure_enough_space(Buffer **buffer_alias)
{
    Buffer *buffer = (*buffer_alias);
    if (buffer->index >= buffer->buffer_size)
    {
        buffer->buffer_size += buffer->grouth_by;
        Buffer *new_buffer = (Buffer *) realloc(buffer, sizeof(Buffer) + sizeof(char) * buffer->buffer_size);
        if (new_buffer)
        {
            *buffer_alias = new_buffer;
            return true;
        }
        else
        {
            printf("Erro alocando memória para buffer");
        }
    }

    return false;
}

bool push(Buffer **buffer_alias, char value)
{
    buffer_ensure_enough_space(buffer_alias);

    Buffer *buffer = (*buffer_alias);

    buffer->buffer[buffer->index] = value;
    buffer->index++;

    return true;
}

bool pop(Buffer *buffer)
{
    if (buffer->index)
    {
        buffer->index--;
        return true;
    }
    return false;
}

const char *null_terminated_buffer(Buffer **buffer_alias)
{
    buffer_ensure_enough_space(buffer_alias);

    Buffer *buffer = (*buffer_alias);

    buffer->buffer[buffer->index] = '\0';

    return &buffer->buffer[0];
}
