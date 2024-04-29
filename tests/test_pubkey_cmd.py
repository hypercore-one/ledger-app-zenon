import pytest

from application_client.zenon_command_sender import ZenonCommandSender, Errors
from application_client.zenon_response_unpacker import unpack_get_public_key_response
from ragger.bip import calculate_public_key_and_chaincode, CurveChoice
from ragger.error import ExceptionRAPDU
from ragger.navigator import NavInsID, NavIns


# In this test we check that the GET_PUBLIC_KEY works in non-confirmation mode
def test_get_public_key_no_confirm(backend):
    for path in ["m/44'/73404'/0'", "m/44'/73404'/1'", "m/44'/73404'/255'"]:
        client = ZenonCommandSender(backend)
        response = client.get_public_key(path=path).data
        _, public_key = unpack_get_public_key_response(response)

        ref_public_key, _ = calculate_public_key_and_chaincode(CurveChoice.Ed25519Slip, path=path)
        assert "00" + public_key.hex() == ref_public_key


# In this test we check that the GET_PUBLIC_KEY works in confirmation mode
def test_get_public_key_confirm_accepted(backend, scenario_navigator):
    client = ZenonCommandSender(backend)
    path = "m/44'/73404'/0'"
    with client.get_public_key_with_confirmation(path=path):
        scenario_navigator.address_review_approve()

    response = client.get_async_response().data
    _, public_key = unpack_get_public_key_response(response)

    ref_public_key, _ = calculate_public_key_and_chaincode(CurveChoice.Ed25519Slip, path=path)
    assert "00" + public_key.hex() == ref_public_key
    #assert chain_code.hex() == ref_chain_code


# In this test we check that the GET_PUBLIC_KEY in confirmation mode replies an error if the user refuses
def test_get_public_key_confirm_refused(backend, scenario_navigator):
    client = ZenonCommandSender(backend)
    path = "m/44'/73404'/0'"

    with pytest.raises(ExceptionRAPDU) as e:
        with client.get_public_key_with_confirmation(path=path):
            scenario_navigator.address_review_reject()

    # Assert that we have received a refusal
    assert e.value.status == Errors.SW_DENY
    assert len(e.value.data) == 0
