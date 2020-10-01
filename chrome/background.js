chrome.tabs.onUpdated.addListener(function (tabId, changeInfo, tab) {
    if (changeInfo.status == 'complete' && tab.url.search('hitomi.la/reader') != -1) {
        chrome.tabs.executeScript(tab.id, {
            code: `
var s = document.createElement('script');
s.src = chrome.runtime.getURL('hitomi.js');
document.head.appendChild(s);
` });
    }
})