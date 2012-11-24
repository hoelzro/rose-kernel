#include <assert.h>
#include <rose/stdint.h>
#include <rose/stream.h>
#include <rose/string.h>

struct memory_stream {
    struct stream stream;
    char memory[256];
    int index;
};

static int
write_char_to_memory(struct stream *stream, char c)
{
    struct memory_stream *mstream = (struct memory_stream *) stream;

    mstream->memory[mstream->index++] = c;

    return 1;
}

static struct memory_stream stream = {
    .stream = {
        .write_char = write_char_to_memory,
    },
};

struct stream *
create_stream(void)
{
    stream.index = 0;

    return (struct stream *) &stream;
}

void
copy_stream_memory(struct stream *stream, char *memory, size_t size)
{
    struct memory_stream *mstream = (struct memory_stream *) stream;

    memcpy(memory, mstream->memory, size);
    memory[size] = '\0';
}

void
destroy_stream(struct stream *stream)
{
    (void) stream;
}

void
verify_large_hex_numbers(void)
{
    struct stream *stream = create_stream();
    char chars[9];

    stream_write_integer(stream, 16, 0x80000011);
    copy_stream_memory(stream, chars, 8);
    assert(memcmp(chars, "80000011", 9) == 0);

    destroy_stream(stream);
}

int
main(void)
{
    verify_large_hex_numbers();

    return 0;
}
