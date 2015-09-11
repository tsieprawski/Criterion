#include <criterion/criterion.h>
#include <criterion/redirect.h>

#include <stdio.h>
#include <ctype.h>

// Testing stdout/stderr

void redirect_all_std(void) {
    cr_redirect_stdout();
    cr_redirect_stderr();
}

Test(redirect, test_outputs, .init = redirect_all_std) {
    FILE* f_stdout = cr_get_redirected_stdout();

    fprintf(stdout, "foo");
    fflush(stdout);

    cr_assert_file_contents_match_str(f_stdout, "foo");

    FILE* f_stderr = cr_get_redirected_stderr();

    fprintf(stderr, "bar");
    fflush(stderr);

    cr_assert_file_contents_match_str(f_stderr, "bar");
}

// Testing general I/O with sample command-line rot13

char rot13_char(char c) {
    return isalpha(c) ? (c - 'a' + 13) % 26 + 'a' : c;
}

void rot13_io(void) {
    char buf[512];

    size_t read;
    while ((read = fread(buf, 1, sizeof (buf), stdin)) > 0) {
        for (size_t i = 0; i < read; ++i)
            buf[i] = rot13_char(buf[i]);
        fwrite(buf, 1, read, stdout);
    }
    fflush(stdout);
}

Test(redirect, rot13, .init = cr_redirect_stdout) {
    FILE* f_stdin = cr_get_redirected_stdin();
    fprintf(f_stdin, "the quick brown fox jumps over the lazy dog");
    fclose(f_stdin);

    rot13_io();

    FILE* f_stdout = cr_get_redirected_stdout();
    cr_assert_file_contents_match_str(f_stdout, "gur dhvpx oebja sbk whzcf bire gur ynml qbt");
}
