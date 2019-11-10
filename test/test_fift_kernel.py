import unittest
import jupyter_kernel_test


class XFiftTests(jupyter_kernel_test.KernelTests):

    kernel_name = 'xeus-fift'
    file_extension = "fif"

    # language_info.name in a kernel_info_reply should match this
    language_name = 'fift'

    # Code in the kernel's language to write "hello, world" to stdout
    code_hello_world = '"hello, world" .'

    # Pager: code that should display something (anything) in the pager
    # code_page_something = "?std::vector"

    # Samples of code which generate a result value (ie, some text
    # displayed as Out[n])
    code_execute_result = [
        {
            'code': '6 7 * .',
            'result': '42'
        }
    ]

    # Samples of code which should generate a rich display output, and
    # the expected MIME type
    # code_display_data = [
    #     {
    #         'code': '#include <string>\n#include "xcpp/xdisplay.hpp"\nstd::string test("foobar");\nxcpp::display(test);',
    #         'mime': 'text/plain'
    #     }
    # ]


if __name__ == '__main__':
    unittest.main()
