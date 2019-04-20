# Script to compress shit into Rar to be uploaded and released.
# It is inteded to be executed from the Project Root Dir

echo "Copying bin folder..."
cp -r bin/ "MHW Costume Armor"

echo "Deleting Extra files..."
rm "MHW Costume Armor"/*.log > /dev/null 2>&1
rm "MHW Costume Armor"/*.a > /dev/null 2>&1
rm "MHW Costume Armor"/SavedSets.json > /dev/null 2>&1
rm "MHW Costume Armor"/Settings.json > /dev/null 2>&1
rm -r "MHW Costume Armor"/Backups > /dev/null 2>&1

echo "Compressing bin folder..."
rm "MHW Costume Armor.rar" > /dev/null 2>&1
/c/"Program Files"/WinRAR/Rar a -ep1 -idq -r -y "MHW Costume Armor.rar" "MHW Costume Armor"

rm -r "MHW Costume Armor"