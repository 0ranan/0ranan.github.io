import * as fs from 'fs';
import * as path from 'path';
import * as readline from 'readline';

// 创建 readline 接口实例
const rl = readline.createInterface({ input: process.stdin, output: process.stdout });

// 提问函数
function inputText(query: string): Promise<string> {
    return new Promise((resolve) => rl.question(query, (answer) => { resolve(answer); }));
}

/** 时间格式化函数 */
function formatDate(date: Date): string {
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0'); // 月份从0开始，需要加1
    const day = String(date.getDate()).padStart(2, '0');
    const hours = String(date.getHours()).padStart(2, '0');
    const minutes = String(date.getMinutes()).padStart(2, '0');
    const seconds = String(date.getSeconds()).padStart(2, '0');

    return `${year}/${month}/${day} ${hours}:${minutes}:${seconds}`;
}

/** 日期格式化函数 */
function formatDay(date: Date): string {
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0'); // 月份从0开始，需要加1
    const day = String(date.getDate()).padStart(2, '0');

    return `${year}${month}${day}`;
}
/**格式化文件报文 */
function makeFileMessage(title: string, tags: string[], comments: string[]): string {
    // 当前时间
    const now = new Date();
    // 标签文本
    let strTags = tags.map((v) => `\n  - ${v}`).join("")
    // 类型文本
    let strComments = comments.map((v) => `\n  - ${v}`).join("")
    return `
---
title: ${title}
date: ${formatDate(now)}
updated: ${formatDate(now)}
comments: true
tags:${strTags}
categories:${strComments}
typora-root-url: ..
---

# ${title}

`
}

async function main() {
    // 获取当前工作目录
    const currentDirectory = process.cwd();
    // 创建文件的路径
    const fileDir = currentDirectory + '/source/_posts'
    // 请输入文件名
    const title = await inputText("enter title:");
    // 输入标签
    const tags = (await inputText("Enter tags separated by '-':")).split('-');
    // 输入类别
    const comments = (await inputText("Enter comments separated by '-':")).split('-');


    // 文件路径和要写入的内容
    const filePath = path.join(fileDir, `${formatDay(new Date())}-${title}.md`);
    // 输入文件路径
    console.log(`file path ${filePath}`);
    // 生成文件主体
    const content = makeFileMessage(title, tags, comments);
    // 文件主体写入内容
    try {
        // 确保目录存在
        fs.mkdirSync(fileDir, { recursive: true });
        fs.writeFileSync(filePath, content);
        // 设置文件权限为 0666
        try {
            fs.chmodSync(filePath, 0o666);
            console.log("File written and permissions set to 666");
        } catch (err) {
            console.warn("File written but failed to set permissions:", err);
        }
    } catch (err) {
        console.error("File write failed", err);
    }
    rl.close();
}

main()