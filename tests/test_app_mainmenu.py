from ragger.firmware import Firmware
from ragger.navigator import NavInsID, NavIns


# In this test we check the behavior of the device main menu
def test_app_mainmenu(firmware, navigator, test_name, default_screenshot_path):
    # Navigate in the main menu
    if firmware.is_nano:
        instructions = [
            NavInsID.RIGHT_CLICK,
            NavInsID.RIGHT_CLICK,
            NavInsID.RIGHT_CLICK
        ]
    elif firmware is Firmware.STAX:
        instructions = [
            NavInsID.USE_CASE_HOME_INFO,
            NavInsID.USE_CASE_SETTINGS_SINGLE_PAGE_EXIT
        ]
    elif firmware is Firmware.FLEX:
        instructions = [
            NavInsID.USE_CASE_HOME_INFO,
            NavInsID.USE_CASE_SETTINGS_SINGLE_PAGE_EXIT
        ]
    navigator.navigate_and_compare(default_screenshot_path, test_name, instructions,
                                   screen_change_before_first_instruction=False)
