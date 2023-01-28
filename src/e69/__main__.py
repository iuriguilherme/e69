"""
e69 - API IOT auto horta  
Copyright (C) 2023 Iuri Guilherme <https://iuri.neocities.org>  

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.  

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.  

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.  
"""

import logging
import os
import sys

logger: logging.Logger = logging.getLogger(__name__)

try:
    from . import name, version, commit
    logger: logging.Logger = logging.getLogger(name)
    try:
        logger.info(f"""Running {name} v{version} ({commit}) with args: \
{sys.argv[1:]}""")
    except:
        logger.critical(f"Running {name} v{version} ({commit})")
    if (len(sys.argv) > 1 and sys.argv[1] in ['production', 'staging']) or \
        os.environ.get('ENV', None) in ['production', 'staging']:
        from .controllers import production
    elif (len(sys.argv) > 1 and sys.argv[1] in ['testing']):
        from .controllers import testing
    else:
        from . import main
        main()
except Exception as e:
    logger.exception(e)
    sys.exit("RTFM")
