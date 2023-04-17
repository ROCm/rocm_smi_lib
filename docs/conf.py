# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

from rocm_docs import ROCmDocs

docs_core = ROCmDocs("ROCm SMI")
docs_core.setup()
docs_core.run_doxygen('.', '.')
docs_core.enable_api_reference()

for sphinx_var in ROCmDocs.SPHINX_VARS:
    globals()[sphinx_var] = getattr(docs_core, sphinx_var)

html_theme_options["show_navbar_depth"] = 2
