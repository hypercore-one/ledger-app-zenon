from ragger.backend.interface import BackendInterface

from application_client.zenon_command_sender import ZenonCommandSender
from application_client.zenon_response_unpacker import unpack_get_app_name_response

from utils import verify_name


# In this test we check that the GET_APP_NAME replies the application name
def test_app_name(backend: BackendInterface) -> None:
    # Use the app interface instead of raw interface
    client = ZenonCommandSender(backend)
    # Send the GET_APP_NAME instruction to the app
    response = client.get_app_name()
    # Assert that we have received the correct appname
    verify_name(unpack_get_app_name_response(response.data))
