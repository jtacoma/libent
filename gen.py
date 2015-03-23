import re
import json
import sys

types = [
    {
        "name": "utf8",
        "varlength": True,
        "data": "char const *"
    },
    {
        "name": "int8",
        "data": "int8_t"
    },
    {
        "name": "int16",
        "data": "int16_t"
    },
    {
        "name": "int32",
        "data": "int32_t"
    },
    {
        "name": "int64",
        "data": "int64_t"
    }
]

templates = {}

templates["buffer_h"] = r"""
/** Copy %(name)s items from an array into a buffer.
 *
 * \param buf buffer to write into
 * \param dst pointer to first %(data)s to copy
 * \param n   number of %(data)s items to copy
 * \return 0 if all's well, -1 otherwise.
 */
int ent_buffer_append_%(name)s (struct ent_buffer * dst, %(data)s const * src, size_t n);
"""

templates["column_h"] = r"""
/** Read %(name)s data from a column.
 *
 * \param      column column to read from
 * \param[out] dst    pointer to read-only internal item array
 * \param[out] n      number of items in array
 * \return 0 if all's well, -1 otherwise.
 */
int ent_column_read_%(name)s (struct ent_column * column, %(data)s const ** dst, size_t * n);
"""

templates["source"] = r"""
int ent_buffer_append_{{ name }} (struct ent_buffer * dst, {{ data }} const * input, size_t n)
{
  assert (0); // not yet implemented
}
"""

def repl(m):
    name = m.group(1)
    if name not in templates:
        raise "no such template: %s" % name
        return m.string
    content = ""
    for t in types:
        content += templates[name] % t
    prefix = m.group(2)
    return "[[[%s]]]\n%s\n%s[[[/%s]]]" % (name, content, prefix, name)
    
def process(text):
    exp = re.compile(
            r"\[\[\[([a-z][a-z_]*)\]\]\].*[\n\r]([^\n\r]*)\[\[\[/\1\]\]\]",
            re.MULTILINE|re.DOTALL)
    return exp.sub(repl, text, re.MULTILINE | re.DOTALL)

for filename in sys.argv[1:]:
    with open(filename, "r") as f:
        original = f.read()
    updated = process(original)
    if updated != original:
        print filename
        with open(filename, "w") as f:
            f.write(updated)
