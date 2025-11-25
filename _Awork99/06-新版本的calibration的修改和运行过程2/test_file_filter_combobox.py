import json
import os
from json import JSONDecodeError
from typing import Set

from PyQt5 import QtWidgets, QtGui, QtCore
from PyQt5.QtCore import pyqtSignal


class TestFileFilterComboBox(QtWidgets.QComboBox):
    test_file_filter_changed = pyqtSignal()

    def __init__(self, cnc_test_app_path, settings_file_folder, test_files_folder):
        super(TestFileFilterComboBox, self).__init__()
        self.view().pressed.connect(self.handleItemPressed)
        self.setModel(QtGui.QStandardItemModel(self))
        self.filters_to_files = {}

        self.load_filters_and_files(cnc_test_app_path, settings_file_folder, test_files_folder)
        self._changed = False

    def handleItemPressed(self, index):
        item = self.model().itemFromIndex(index)
        if item.checkState() == QtCore.Qt.Checked:
            item.setCheckState(QtCore.Qt.Unchecked)
        else:
            item.setCheckState(QtCore.Qt.Checked)
        self._changed = True
        self.test_file_filter_changed.emit()

    def hidePopup(self):
        if not self._changed:
            super(TestFileFilterComboBox, self).hidePopup()
        self._changed = False

    def load_filters_and_files(self, cnc_test_app_path, settings_file_folder, test_files_folder):
        # first find the fixture ID
        fixture_id = self.get_fixture_id(cnc_test_app_path)

        # now find the fixture type
        fixture_type = self.get_fixture_type(settings_file_folder, fixture_id)

        # now go through each of the test files
        for root, dirs, files in os.walk(test_files_folder):
            relative_path = os.path.relpath(root, test_files_folder)
            if relative_path == ".":
                relative_path = "Production"
            compatible_files = set()
            for filename in files:
                with open(os.path.join(root, filename)) as f:
                    try:
                        data = json.load(f)
                        if "fixture_type" in data:
                            if fixture_type in data["fixture_type"]:
                                compatible_files.add(filename)
                        else:
                            # if no fixture_type is specified, treat it as all
                            compatible_files.add(filename)
                    except JSONDecodeError:
                        print(f"Cannot load {filename} in {root} as json")
            if len(compatible_files) > 0:
                self.filters_to_files[relative_path] = compatible_files
                self.addItem(relative_path)
                item = self.model().item(self.count() - 1, 0)
                item.setCheckState(QtCore.Qt.Unchecked)

    def get_fixture_id(self, cnc_test_app_path) -> str:
        config_path = os.path.join(os.path.dirname(cnc_test_app_path), "config.json")
        if not os.path.exists(config_path):
            raise ValueError(f"Unable to find config.json in {os.path.dirname(cnc_test_app_path)}")
        with open(config_path) as config_file:
            try:
                data = json.load(config_file)
                if "FixtureID" in data:
                    return data["FixtureID"]
                else:
                    raise ValueError("Unable to find FixtureID in config.json")
            except JSONDecodeError:
                print(f"Cannot load {config_path} as json")
                raise


    def get_fixture_type(self, settings_file_folder, fixture_id):
        # find the settings file for this fixture
        #settings_file_path = os.path.join(settings_file_folder, f"cnctestapplication_settings_{fixture_id}.json")
        settings_file_path = os.path.join(settings_file_folder, f"cnctestapplication_settings_14.json")
        if not os.path.exists(settings_file_path):
            raise ValueError(f"Unable to find {settings_file_path}")
        with open(settings_file_path) as settings_file:
            try:
                data = json.load(settings_file)
                if "FixtureType" in data:
                    return data["FixtureType"]
                else:
                    raise ValueError(f"Unable to find FixtureType in {settings_file_path}")
            except JSONDecodeError:
                print(f"Cannot load {settings_file_path} as json")
                raise

    def load_checked_from_string(self, comma_separated_items: str):
        checked_items = comma_separated_items.split(",")
        for i in range(self.count()):
            item = self.model().item(i, self.modelColumn())
            if item.text() in checked_items:
                item.setCheckState(QtCore.Qt.Checked)
            else:
                item.setCheckState(QtCore.Qt.Unchecked)

    def get_checked_items_string(self):
        all_items = [self.model().item(i, self.modelColumn()) for i in range(self.count())]
        checked_item_texts = [item.text() for item in all_items if item.checkState() == QtCore.Qt.Checked]
        return ','.join(checked_item_texts)

    def get_compatible_files_for_suffix(self, suffix: str) -> [str]:
        # go through all the filters, if checked, get the files that match the suffix
        results = []
        for i in range(self.count()):
            item = self.model().item(i, self.modelColumn())
            if item.checkState() == QtCore.Qt.Checked:
                if item.text() == "Production":
                    files = [filename for filename in self.filters_to_files[item.text()] if filename.endswith(suffix)]
                else:
                    files = [os.path.join(item.text(), filename) for filename in self.filters_to_files[item.text()] if
                             filename.endswith(suffix)]
                results.extend(files)
        return results

class Dialog_01(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()
        myQWidget = QtWidgets.QWidget()
        myBoxLayout = QtWidgets.QHBoxLayout()
        myQWidget.setLayout(myBoxLayout)
        self.setCentralWidget(myQWidget)
        self.ComboBox = TestFileFilterComboBox("c:\\workspace\\cnctestapplication\\bin\\Release\\CNCTestApplication.exe",
                                               "c:\\workspace\\cnctestapplication\\cnctestapplicationtestfiles\\SettingsFiles",
                                               "c:\\workspace\\cnctestapplication\\cnctestapplicationtestfiles\\TestFiles")
        myBoxLayout.addWidget(self.ComboBox)


if __name__ == '__main__':
    app = QtWidgets.QApplication(['Test'])
    dialog_1 = Dialog_01()
    dialog_1.show()
    app.exec_()

