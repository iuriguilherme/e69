"""
e69 - API IOT auto horta  
Copyright (C) 2023 Iuri Guilherme <https://iuri.neocities.org>  

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.  

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.  

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.  
"""

import logging
from fastapi import FastAPI
from typing import Union
import uvicorn

name: str = 'e69'
description: str = "API para sistema de automatização de horta urbana"
version: str = '0.0.0.0'
commit: str = '0000000'

logging.basicConfig(level = "INFO")
logger: logging.Logger = logging.getLogger(name)
logger.critical("""
    e69 - API IOT auto horta
    Copyright (C) 2023 Iuri Guilherme <https://iuri.neocities.org>
    This program comes with ABSOLUTELY NO WARRANTY;
    This is free software, and you are welcome to redistribute it
    under certain conditions;
    See the LICENSE file for details.
""")

try:
    from . import _version
    version: str = _version.__version__
except:
    logger.debug(f"""Unable to get version from _version file, using \
{version}""")
try:
    ## Using latest git tag as version
    version: str = natsort.natsorted(os.listdir('.git/refs/tags'))[-1]
except:
    logger.debug(f"Latest git tag not found, version will be {version}")
try:
    ## Lookup latest commit hash
    with open(os.path.abspath('.git/HEAD')) as git_head:
        git_head.seek(5)
        with open('.git/' + git_head.read().strip('\n')) as git_ref:
            commit = git_ref.read(7)
except:
    logger.debug(f"git repository not found, commit will be {commit}")

__name__: str = name
__version__: str = version
__description__: str = description

__all__: list = [
    name,
    __version__,
    __description__,
]

class Config():
    """Configuration model"""
    log_level: str = 'DEBUG'
    ## Uvicorn
    # ~ socket: Union[str, None] = "uvicorn.sock"
    socket: Union[str, None] = None
    forwarded_allow_ips: str = "*"
    timeout_keep_alive: int = 0
    host: Union[str, None] = "localhost"
    port: Union[int, None] = 8000
    reload: bool = False

api: FastAPI = FastAPI()

@api.get("/")
async def root():
    return {"status": True, "message": "Deu certo!"}

def main(
    *args,
    config: Union[Config, None] = None,
    app: Union[FastAPI, None] = api,
    socket: Union[str, None] = None,
    **kwargs,
) -> None:
    """main"""
    try:
        if config is None:
            config: Config = Config()
        logger.setLevel(getattr(logging, config.log_level.upper()))
        if socket is not None:
            config.socket = socket
    except Exception as e:
        logger.exception(e)
        return
    try:
        uvicorn.run(
            app,
            uds = config.socket,
            forwarded_allow_ips = config.forwarded_allow_ips,
            proxy_headers = True,
            timeout_keep_alive = config.timeout_keep_alive,
            log_level = config.log_level.lower(),
            reload = config.reload,
        )
    except Exception as e:
        logger.exception(e)
        try:
            uvicorn.run(
                app,
                host = config.host,
                port = int(config.port),
                log_level = config.log_level.lower(),
                reload = config.reload,
            )
        except Exception as e:
            logger.error("""Neither socket or hostname is set in \
configuration file, uvicorn therefore can't start!""")
            logger.exception(e)
    logger.info(f"Finishing {name}")

if __name__ == '__main__':
    main(Config(), api)
