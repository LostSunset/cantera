# This file is part of Cantera. See License.txt in the top-level directory or
# at https://cantera.org/license.txt for license and copyright information.

from dataclasses import dataclass
from typing import Dict


@dataclass(frozen=True)
class Config:
    """Provides configuration info for the CSharpSourceGenerator class"""


    ret_type_crosswalk = {
        "const char*": "string",
        "const double*": "double[]",
        "const int*": "int[]",
        "size_t": "nuint",
        "char*": "byte*"
    }

    prop_type_crosswalk = {
        "byte*": "string",
        "double*": "double[]"
    }

    # These we load from the parsed YAML config file
    class_crosswalk: Dict[str, str]

    class_accessors: Dict[str, str]

    derived_handles: Dict[str, str]

    wrapper_classes: Dict[str, Dict[str, str]]

    @classmethod
    def from_parsed(cls, *,
                    class_crosswalk=None, class_accessors=None,
                    derived_handles=None, wrapper_classes=None):
        return cls(class_crosswalk or {}, class_accessors or {},
                   derived_handles or {}, wrapper_classes or {})
