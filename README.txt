SPRESENSE + Neural Network Console を用いた画像認識楽器

基本 using_file以下のファイルを使って下さい

data : データとして用いる画像セット
output : データから作成したデータセット
meiji.sdcproj : dataからこちらで適当に学習させた結果
meiji.files : dataからこちらで適当に学習させた結果の出力（model.nnbをSDカードに差し込めば一応動かせます）
spresense : Spresenseに書き込むinoファイル（分類結果をもとに音を出します）
