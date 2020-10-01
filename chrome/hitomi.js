async function downloadFile(url, filename)
{
    try {
        let res = await fetch(url);
        if (res.ok) {
            let data = await res.blob();
            let u = window.URL.createObjectURL(data);
            let a = document.createElement('a');
            a.style.display = 'none';
            a.href = u;
            a.download = filename;
            document.body.appendChild(a);
            a.click();
            window.URL.revokeObjectURL(u);
            return true;
        }
    }
    catch (e) {
        console.log(e);
    }
    return false;
}

async function downloadImages(start = 0, end = -1)
{
    let title = document.title.replace(" | Hitomi.la", "").replaceAll(" ", "_");
    let gid = galleryinfo.id;

    let numFiles = galleryinfo.files.length;
    end = end < 0 ? numFiles : Math.min(numFiles, end);

    let numCompleted = 0;
    for (let fi = start; fi < end; ++fi) {
        let info = our_galleryinfo[fi];
        let url = url_from_url_from_hash(gid, info);
        let filename = title + "_" + info.name;

        if (await downloadFile(url, filename)) {
            ++numCompleted;
            console.log((fi + 1).toString() + "/" + end.toString() + " " + filename + " : " + url);
        }
    }
    return numCompleted;
}

