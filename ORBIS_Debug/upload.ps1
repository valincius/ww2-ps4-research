$root = $PSScriptRoot

$file = "WW2-PS4-PRX"
$remotePath = "/mnt/usb0/"

python2 "$root/make_fself.py" "$root/$file.prx" "$root/$file.sprx"

$ftp = [System.Net.FtpWebRequest]::Create("ftp://192.168.1.21:9905/$remotePath/$file.sprx")
$ftp.Method = [System.Net.WebRequestMethods+Ftp]::UploadFile
$ftp.Credentials = new-object System.Net.NetworkCredential("anonymous","anonymous@localhost")
$ftp.UseBinary = $true
$ftp.UsePassive = $true

$content = [System.IO.File]::ReadAllBytes("$root/$file.sprx")
$ftp.ContentLength = $content.Length

$rs = $ftp.GetRequestStream()
$rs.Write($content, 0, $content.Length)

$rs.Close()
$rs.Dispose()

"Uploaded!"