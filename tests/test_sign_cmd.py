import pytest
import hashlib

from application_client.zenon_transaction import Transaction
from application_client.zenon_command_sender import ZenonCommandSender, Errors
from application_client.zenon_response_unpacker import unpack_get_public_key_response, unpack_sign_tx_response
from ragger.error import ExceptionRAPDU
from ragger.navigator import NavInsID
from utils import ROOT_SCREENSHOT_PATH, check_signature_validity

# In this tests we check the behavior of the device when asked to sign a transaction


# In this test we send to the device a transaction to sign and validate it on screen
# The transaction is a receive transaction and will be sent in multiple chunks
# We will ensure that the displayed information is correct by using screenshots comparison
def test_sign_tx_send_tx(firmware, backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = ZenonCommandSender(backend)
    # The path used for this entire test
    path: str = "m/44'/73404'/0'"

    # First we need to get the public key of the device in order to build the transaction
    rapdu = client.get_public_key(path=path)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    # Create the transaction that will be sent to the device for signing
    # Tx 1 = Send 999 ZNN to z1qqjnwjjpnue8xmmpanz6csze6tcmtzzdtfsww7
    transaction = bytes.fromhex("0000000000000001000000000000014100000000000000028ecf09ffd844931d45884b7f408f1197ff9d80f0a4c02110a25849c7509bbb6200000000000003e88dcc62a525b6ccd452087612a4c4217427cab6b9dae983fdec8be3aaa3ad8f340000000000052b8e000c6caaee177e9158ace125e6bb7d94cac250410025374a419f32736f61ecc5ac4059d2f1b5884d000000000000000000000000000000000000000000000000000000174281070014e66318c6318c6318c60000000000000000000000000000000000000000000000000000000000000000a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434aa7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a000000000000520800000000000000000000000000000000")

    # Tx 2 = Send 666.999 QSR to z1qqjnwjjpnue8xmmpanz6csze6tcmtzzdtfsww7
    # transaction = bytes.fromhex("0000000000000001000000000000014100000000000000028ecf09ffd844931d45884b7f408f1197ff9d80f0a4c02110a25849c7509bbb6200000000000003e88dcc62a525b6ccd452087612a4c4217427cab6b9dae983fdec8be3aaa3ad8f340000000000052b8e000c6caaee177e9158ace125e6bb7d94cac250410025374a419f32736f61ecc5ac4059d2f1b5884d0000000000000000000000000000000000000000000000000000000f879fb46004066318c6318c6318c60000000000000000000000000000000000000000000000000000000000000000a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434aa7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a000000000000520800000000000000000000000000000000")

    # Tx 3 = Send 9 INCH to z1qqjnwjjpnue8xmmpanz6csze6tcmtzzdtfsww7
    # transaction = bytes.fromhex("0000000000000001000000000000014100000000000000028ecf09ffd844931d45884b7f408f1197ff9d80f0a4c02110a25849c7509bbb6200000000000003e88dcc62a525b6ccd452087612a4c4217427cab6b9dae983fdec8be3aaa3ad8f340000000000052b8e000c6caaee177e9158ace125e6bb7d94cac250410025374a419f32736f61ecc5ac4059d2f1b5884d0000000000000000000000000000000000000000000000000000000035a4e90001df1c3e6da6132543750000000000000000000000000000000000000000000000000000000000000000a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434aa7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a000000000000520800000000000000000000000000000000")

    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    with client.sign_tx(path=path, transaction=transaction):
        # Validate the on-screen request by performing the navigation appropriate for this device
        if firmware.device.startswith("nano"):
            navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                    [NavInsID.BOTH_CLICK],
                                                    "Approve",
                                                    ROOT_SCREENSHOT_PATH,
                                                    test_name)
        else:
            navigator.navigate_until_text_and_compare(NavInsID.USE_CASE_REVIEW_TAP,
                                                    [NavInsID.USE_CASE_REVIEW_CONFIRM,
                                                    NavInsID.USE_CASE_STATUS_DISMISS],
                                                    "Hold to sign",
                                                    ROOT_SCREENSHOT_PATH,
                                                    test_name)

    # The device as yielded the result, parse it and ensure that the signature is correct
    response = client.get_async_response().data
    _, signature = unpack_sign_tx_response(response)

    txHash = hashlib.sha3_256()
    txHash.update(transaction)

    assert check_signature_validity(public_key, signature, txHash.digest())


# In this test we send to the device a transaction to sign and validate it on screen
# The transaction is a send transaction and will be sent in multiple chunks
# We will ensure that the displayed information is correct by using screenshots comparison
def test_sign_tx_receive_tx(firmware, backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = ZenonCommandSender(backend)
    path: str = "m/44'/73404'/0'"

    rapdu = client.get_public_key(path=path)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    transaction = bytes.fromhex("0000000000000001000000000000014100000000000000018ecf09ffd844931d45884b7f408f1197ff9d80f0a4c02110a25849c7509bbb6200000000000003de8dcc62a525b6ccd452087612a4c4217427cab6b9dae983fdec8be3aaa3ad8f340000000000052b8e000c6caaee177e9158ace125e6bb7d94cac250410025374a419f32736f61ecc5ac4059d2f1b5884d00000000000000000000000000000000000000000000000000000002540be40014e66318c6318c6318c60000000000000000000000000000000000000000000000000000000000000000a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434aa7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a000000000000520800000000000000000000000000000000")

    with client.sign_tx(path=path, transaction=transaction):
        if firmware.device.startswith("nano"):
            navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                    [NavInsID.BOTH_CLICK],
                                                    "Approve",
                                                    ROOT_SCREENSHOT_PATH,
                                                    test_name)
        else:
            navigator.navigate_until_text_and_compare(NavInsID.USE_CASE_REVIEW_TAP,
                                                    [NavInsID.USE_CASE_REVIEW_CONFIRM,
                                                    NavInsID.USE_CASE_STATUS_DISMISS],
                                                    "Hold to sign",
                                                    ROOT_SCREENSHOT_PATH,
                                                    test_name)
    response = client.get_async_response().data
    _, signature = unpack_sign_tx_response(response)

    txHash = hashlib.sha3_256()
    txHash.update(transaction)

    assert check_signature_validity(public_key, signature, txHash.digest())


# Transaction signature refused test
# The test will ask for a transaction signature that will be refused on screen
def test_sign_tx_refused(firmware, backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = ZenonCommandSender(backend)
    path: str = "m/44'/73404'/0'"

    rapdu = client.get_public_key(path=path)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    # Tx 1 = Send 999 ZNN to z1qqjnwjjpnue8xmmpanz6csze6tcmtzzdtfsww7
    transaction = bytes.fromhex("0000000000000001000000000000014100000000000000028ecf09ffd844931d45884b7f408f1197ff9d80f0a4c02110a25849c7509bbb6200000000000003e88dcc62a525b6ccd452087612a4c4217427cab6b9dae983fdec8be3aaa3ad8f340000000000052b8e000c6caaee177e9158ace125e6bb7d94cac250410025374a419f32736f61ecc5ac4059d2f1b5884d000000000000000000000000000000000000000000000000000000174281070014e66318c6318c6318c60000000000000000000000000000000000000000000000000000000000000000a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434aa7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a000000000000520800000000000000000000000000000000")

    if firmware.device.startswith("nano"):
        with pytest.raises(ExceptionRAPDU) as e:
            with client.sign_tx(path=path, transaction=transaction):
                navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                          [NavInsID.BOTH_CLICK],
                                                          "Reject",
                                                          ROOT_SCREENSHOT_PATH,
                                                          test_name)

        # Assert that we have received a refusal
        assert e.value.status == Errors.SW_DENY
        assert len(e.value.data) == 0
    else:
        for i in range(3):
            instructions = [NavInsID.USE_CASE_REVIEW_TAP] * i
            instructions += [NavInsID.USE_CASE_REVIEW_REJECT,
                             NavInsID.USE_CASE_CHOICE_CONFIRM,
                             NavInsID.USE_CASE_STATUS_DISMISS]
            with pytest.raises(ExceptionRAPDU) as e:
                with client.sign_tx(path=path, transaction=transaction):
                    navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH,
                                                   test_name + f"/part{i}",
                                                   instructions)
            # Assert that we have received a refusal
            assert e.value.status == Errors.SW_DENY
            assert len(e.value.data) == 0
